#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include "func.h"
#include "lz.h"

struct diretorio *inicializa_arquivo(char *arquivo)
{
  struct stat info;
  struct diretorio *d = malloc(sizeof(struct diretorio));
  if (!d)
    return NULL;

  if (stat(arquivo, &info) != 0) 
  {
    printf("Erro ao acessar o arquivo\n");
    free(d);
    return NULL;
  }

  strncpy(d->nome, arquivo, 99);
  d->nome[99] = '\0';
  d->UID = info.st_uid;
  d->tamanho_og = info.st_size;
  d->tamanho_disc = info.st_size;
  d->data = info.st_mtime; 
  d->ordem = -1;
  d->local = -1;

  return d;
}

int le_diretorio(struct diretorio **diretorios, FILE *archive)
{
  long int tam_archive, tam_diretorio, dir_inicio;
  int tam, i;

  if (!diretorios || !archive)
    return -1;

  fseek(archive, 0, SEEK_END);
  tam_archive = ftell(archive);
  if (tam_archive < (long int)sizeof(int))
    return 0;
  
  fseek(archive, -sizeof(int), SEEK_END); //achar o tamanho do diretório
  if (fread(&tam, sizeof(int), 1, archive) != 1) //se deu errado a leitura // também pega o valor e armazena em tam
    return 0;

  tam_diretorio = (sizeof(int) + tam * sizeof(struct diretorio));

  dir_inicio = tam_archive - tam_diretorio;

  fseek(archive, dir_inicio, SEEK_SET); //vai até o começo do diretório
  for (i = 0; i < tam; i++)
  {
    if (diretorios[i])
      free(diretorios[i]);
    diretorios[i] = malloc(sizeof(struct diretorio));
    fread(diretorios[i], sizeof(struct diretorio), 1, archive);
  }

  return tam;
}

void escreve_diretorio(struct diretorio **diretorios, int n, FILE *archive)
{
  long int tam_archive, dir_inicio, tam_diretorio;

  if (!diretorios || !archive)
    return;
  
  fseek(archive, 0, SEEK_END);
  tam_archive = ftell(archive);
  tam_diretorio = (sizeof(int) + n * sizeof(struct diretorio));
  dir_inicio = tam_archive - tam_diretorio;
  fseek(archive, dir_inicio, SEEK_SET); //onde começa o diretorio
  ftruncate(fileno(archive), dir_inicio);

  for (int i = 0; i < n; i++)
    fwrite(diretorios[i], sizeof(struct diretorio), 1, archive);

  fwrite(&n, sizeof(int), 1, archive);

  return;
}

void destroi_diretorios(struct diretorio **diretorios, int n)
{
  if (!diretorios)
    return;

  for (int i = 0; i < n; i++)
  {
   if (diretorios[i])
     free(diretorios[i]);
  }

  free(diretorios);

  return;
}

void opcao_ip(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{
  FILE *file;
  long int bytes, offset;
  char buffer[1024];
  int tam, i, repetido, ordem;
  repetido = ordem = 0;

  if (!arquivo || !archive)
    return;
  
  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
    printf("Erro na leitura do diretório\n");

  for (i = 0; i < tam; i++)
  {
    if (strcmp(diretorios[i]->nome, arquivo->nome) == 0)
      repetido = 1; 
  }
  
  if (repetido)
  {
    printf("Arquivo repetido\n"); //temporário
    return;
  }

  file = fopen(arquivo->nome, "rb");
  if (!file)
    printf("Erro ao abrir o arquivo %s\n", arquivo->nome);

  fseek(archive, 0, SEEK_END);
  if (tam == 0) //archive vazio
  {
    ftruncate(fileno(archive), 0); //exclui lixos de memória
    fseek(archive, 0, SEEK_SET);
    offset = ftell(archive); //posição do arquivo
    arquivo->local = offset;
    bytes = fread(buffer, 1, sizeof(buffer), file);
    while (bytes > 0) //escreve os bytes do file em archive
    {
      fwrite(buffer, 1, bytes, archive);
      bytes = fread(buffer, 1, sizeof(buffer), file);
    }
    arquivo->tamanho_disc = arquivo->tamanho_og;
    arquivo->ordem = 0;
    diretorios[0] = arquivo;
  }
  else if (!repetido)
  {
    offset = 0;
    for (i = 0; i < tam; i++)
    {
      offset = offset + diretorios[i]->tamanho_disc; //pega o tamanho de todos os arquivos armazenados
      ordem++;
    }
    ftruncate(fileno(archive), offset); //trunca o archive sem o diretorio
    fseek(archive, offset, SEEK_SET);
    arquivo->local = offset;
    bytes = fread(buffer, 1, sizeof(buffer), file);
    while (bytes > 0) //escreve os bytes do file em archive
    {
      fwrite(buffer, 1, bytes, archive);
      bytes = fread(buffer, 1, sizeof(buffer), file);
    }
    arquivo->tamanho_disc = arquivo->tamanho_og;
    arquivo->ordem = ordem;
    diretorios[ordem] = arquivo;
  }

  fseek(archive, 0, SEEK_END);
  escreve_diretorio(diretorios, (ordem + 1), archive);
  fclose(file);
  return;
}

void opcao_ic(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{
  FILE *file, *new;
  long int bytes, bytes_lidos, offset, og_size, new_size;
  char buffer[1024], *conteudo, *new_conteudo;
  int tam, i, repetido, ordem, comprimido;
  repetido = ordem = 0;
  comprimido = 1;

  if (!arquivo || !archive)
    return;
  
  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
    printf("Erro na leitura do diretório\n");

  for (i = 0; i < tam; i++)
  {
    if (strcmp(diretorios[i]->nome, arquivo->nome) == 0)
      repetido = 1;
  }
  
  if (repetido)
  {
    printf("Arquivo repetido\n"); //temporário
    return;
  }

  file = fopen(arquivo->nome, "rb");
  if (!file)
    printf("Erro ao abrir o arquivo %s\n", arquivo->nome);

  fseek(file, 0, SEEK_END);
  og_size = ftell(file);
  conteudo = malloc(og_size);
  if (!conteudo)
  {
    printf("Erro ao alocar conteudo de arquivo para comprimir\n");
    fclose(file);
    return;
  }
  new_conteudo = malloc(2 * og_size); //a compressão pode ser maior
  if (!new_conteudo)
  {
    printf("Erro ao alocar conteudo de arquivo para comprimir\n");
    free(conteudo);
    fclose(file);
    return;
  }
  fseek(file, 0, SEEK_SET);
  bytes_lidos = fread(conteudo, 1, og_size, file);
  new_size = LZ_Compress((unsigned char *)conteudo,(unsigned char *)new_conteudo, bytes_lidos);
  new = fopen("temp.o", "wb");
  fwrite(new_conteudo, 1, new_size, new); 
  
  if (new_size > og_size)
  {
    comprimido = 0;
    arquivo->tamanho_disc = og_size;
  }
  else 
  {
    comprimido = 1;
    arquivo->tamanho_disc = new_size;
  }

  fseek(archive, 0, SEEK_END);
  if (tam == 0) //archive vazio
  {
    ftruncate(fileno(archive), 0); //exclui lixos de memória
    fseek(archive, 0, SEEK_SET);
    offset = ftell(archive); //posição do arquivo
    arquivo->local = offset;
    if (comprimido == 0)
    {  
      bytes = fread(buffer, 1, sizeof(buffer), file);
      while (bytes > 0) //escreve os bytes do file em archive
      {
        fwrite(buffer, 1, bytes, archive);
        bytes = fread(buffer, 1, sizeof(buffer), file);
      }
    }
    else
    {
      bytes = fread(buffer, 1, sizeof(buffer), new);
      while (bytes > 0) //escreve os bytes do file em archive
      {
        fwrite(buffer, 1, bytes, archive);
        bytes = fread(buffer, 1, sizeof(buffer), new);
      }
    }
    arquivo->ordem = 0;
    diretorios[0] = arquivo;
  }
  else if (!repetido)
  {
    offset = 0;
    for (i = 0; i < tam; i++)
    {
      offset = offset + diretorios[i]->tamanho_disc; //pega o tamanho de todos os arquivos armazenados
      ordem++;
    }
    ftruncate(fileno(archive), offset); //trunca o archive sem o diretorio
    fseek(archive, offset, SEEK_SET);
    arquivo->local = offset;
    if (comprimido == 0)
    {  
      bytes = fread(buffer, 1, sizeof(buffer), file);
      while (bytes > 0) //escreve os bytes do file em archive
      {
        fwrite(buffer, 1, bytes, archive);
        bytes = fread(buffer, 1, sizeof(buffer), file);
      }
    }
    else
    {
      bytes = fread(buffer, 1, sizeof(buffer), new);
      while (bytes > 0) //escreve os bytes do file em archive
      {
        fwrite(buffer, 1, bytes, archive);
        bytes = fread(buffer, 1, sizeof(buffer), new);
      }
    }
    arquivo->ordem = ordem;
    diretorios[ordem] = arquivo;
  }

  fseek(archive, 0, SEEK_END);
  escreve_diretorio(diretorios, (ordem + 1), archive);
  fclose(file);
  free(conteudo);
  free(new_conteudo);
  return;
}

void opcao_m(char *arquivo, char *target, FILE *archive, struct diretorio **diretorios)
{
  int tam, i, num, num_target;
  long int offset, size;
  char *buffer, *aux;
  struct diretorio *temp;

  if (!archive || !diretorios || !arquivo)
    return;

  if (target && strcmp(arquivo, target) == 0) //caso target e arquivo sejam o mesmo
  {
    printf("Posição do membro e destino do membro são iguais\n");
    return;
  }

  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
    printf("Erro na leitura do diretório\n");

  if (tam == 0)
  {
    printf("Sem membros para mover\n");
    return;
  }

  num = -1;
  for (i = 0; i < tam; i++)
  {
    if (strcmp(diretorios[i]->nome, arquivo) == 0)
      num = i;
  }
  if (num == -1)
  {
    printf("Arquivo não está no archive\n");
    return;
  }

  offset = diretorios[num]->local;
  size = diretorios[num]->tamanho_disc;
  buffer = malloc(size);
  if(!buffer)
  {
    printf("Erro ao alocar buffer para ler arquivo para mover\n");
    return;
  }

  fseek(archive, offset, SEEK_SET);
  fread(buffer, 1, size, archive); //armazena em buffer o conteúdo do membro a ser movido

  if (!target) //move para o começo
  {
    for(i = num - 1; i >= 0; i--)
    {
      aux = malloc(diretorios[i]->tamanho_disc);
      if (!aux)
      {
        printf("Erro ao alocar buffer para mover um arquivo em -m\n");
        free(buffer);
        return;
      }
      fseek(archive, diretorios[i]->local, SEEK_SET);
      fread(aux, 1, diretorios[i]->tamanho_disc, archive);
      diretorios[i]->local = diretorios[i]->local + diretorios[num]->tamanho_disc;
      fseek(archive, diretorios[i]->local, SEEK_SET);
      fwrite(aux, 1, diretorios[i]->tamanho_disc, archive);
      free(aux);
    }
    fseek(archive, 0, SEEK_SET);
    fwrite(buffer, 1, diretorios[num]->tamanho_disc, archive);
    free(buffer);
    diretorios[num]->local = 0;
    temp = diretorios[num];  //atualizando o diretorio
    for (i = num; i > 0; i--)
      diretorios[i] = diretorios[i - 1];
    diretorios[0] = temp;
    escreve_diretorio(diretorios, tam, archive);
    return;
  }
  else //target existe
  {
    for (i = 0; i < tam; i++) //acha o target
    { 
      if (strcmp(diretorios[i]->nome, target) == 0)
        num_target = i;
    }
    if (diretorios[num_target]->ordem > diretorios[num]->ordem) //caso 1: arquivo a ser movido está antes do destino
    {
      for (i = num + 1; i <= num_target; i++)
      {
        aux = malloc(diretorios[i]->tamanho_disc);
        if (!aux)
        {
          printf("Erro ao alocar buffer para mover um arquivo em -m\n");
          free(buffer);
          return;
        }
        fseek(archive, diretorios[i]->local, SEEK_SET);
        fread(aux, 1, diretorios[i]->tamanho_disc, archive);
        diretorios[i]->local = diretorios[i]->local - diretorios[num]->tamanho_disc;
        fseek(archive, diretorios[i]->local, SEEK_SET);
        fwrite(aux, 1, diretorios[i]->tamanho_disc, archive);
        free(aux);
      }
      diretorios[num]->local = diretorios[num_target]->local + diretorios[num_target]->tamanho_disc;
      fseek(archive, diretorios[num]->local, SEEK_SET);
      fwrite(buffer, 1, diretorios[num]->tamanho_disc, archive);
      free(buffer);
      temp = diretorios[num];
      for (i = num; i < num_target; i++)
        diretorios[i] = diretorios[i + 1];
      diretorios[num_target] = temp;
      escreve_diretorio(diretorios, tam, archive);
    }
    else //caso 2: arquivo a ser movido está depois do destino
    {
      for (i = num - 1; i > num_target; i--)
      {
        aux = malloc(diretorios[i]->tamanho_disc);
        if (!aux)
        {
          printf("Erro ao alocar buffer para mover um arquivo em -m\n");
          free(buffer);
          return;
        }
        fseek(archive, diretorios[i]->local, SEEK_SET);
        fread(aux, 1, diretorios[i]->tamanho_disc, archive);
        diretorios[i]->local = diretorios[i]->local + diretorios[num]->tamanho_disc;
        fseek(archive, diretorios[i]->local, SEEK_SET);
        fwrite(aux, 1, diretorios[i]->tamanho_disc, archive);
        free(aux);
      }
      diretorios[num]->local = diretorios[num_target]->local + diretorios[num_target]->tamanho_disc;
      fseek(archive, diretorios[num]->local, SEEK_SET);
      fwrite(buffer, 1, diretorios[num]->tamanho_disc, archive);
      free(buffer);
      temp = diretorios[num];
      for (i = num; i > num_target + 1; i--)
        diretorios[i] = diretorios[i - 1];
      diretorios[num_target + 1] = temp;
      escreve_diretorio(diretorios, tam, archive);
    }
  }
  return;
}

void opcao_x(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{
  FILE *file; //falta verificar se o arquivo é comprimido ou não e descomprimir se for o caso
  char buffer[1024];
  long int bytes;
  int num, i, tam;

  if (!archive)
    return;

  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
  {
    printf("Erro na leitura do diretório\n");
    return;
  }
  else if (tam == 0)
  {
    printf("Sem arquivos no archive\n");
    return;
  }

  if (arquivo)
  {
    num = -1;
    for (i = 0; i < tam; i++)
    {
      if (strcmp(diretorios[i]->nome, arquivo->nome) == 0)
        num = i;
    }
    if (num == -1)
    {
      printf("Arquivo não está no archive\n");
      return;
    }
  
    file = fopen(arquivo->nome, "rb+");
    if (!file)
    {
      printf("Erro ao abrir o arquivo %s\n", arquivo->nome);
      return;
    }
  
    fseek(archive, diretorios[num]->local, SEEK_SET); 
    bytes = fread(buffer, 1, sizeof(buffer), archive);
    while (bytes > 0)
    {
      fwrite(buffer, 1, bytes, file);
      bytes = fread(buffer, 1, sizeof(buffer), archive);
    }

    fclose(file);
  }
  else //extrai todos
  {
    for(i = 0; i < tam; i++)
    {
      fseek(archive, diretorios[i]->local, SEEK_SET);
      file = fopen(diretorios[i]->nome, "wb");
      bytes = fread(buffer, 1, sizeof(buffer), archive);
      while (bytes > 0)
      {
        fwrite(buffer, 1, bytes, file);
        bytes = fread(buffer, 1, sizeof(buffer), archive);
      }
      fclose(file);
    }
  }

  return;
}

void opcao_r(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{
  char *buffer; 
  int i, num, tam;
  long int fim;
  num = -1;

  if (!arquivo || !archive || !diretorios)
    return;

  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
    printf("Erro na leitura do diretório\n");

  for (i = 0; i < tam; i++) //acha o arquivo a ser removido do archive
  {
    if (strcmp(diretorios[i]->nome, arquivo->nome) == 0)
      num = i;
  }
 
  if (num == -1)
  {
    printf("Arquivo não está no archive\n");
    return;
  }

  for (i = num + 1; i < tam; i++)
  {
    buffer = malloc(sizeof(diretorios[i]->tamanho_disc));
    if (!buffer)
    {
      printf("Erro ao alocar memória na remoção de arquivo\n");
      return;
    }
    fseek(archive, diretorios[i]->local, SEEK_SET); //acha arquivo 
    fread(buffer, 1, diretorios[i]->tamanho_disc, archive); //lê conteúdo
    diretorios[i]->local = diretorios[i]->local - diretorios[num]->tamanho_disc; //diminue no offset o tam do arquivo que sai
    fseek(archive, diretorios[i]->local, SEEK_SET);
    fwrite(buffer, 1, diretorios[i]->tamanho_disc, archive);
    free(buffer);
  }

  for (i = num; i < tam - 1; i++) //arruma a ordem do arquivos que estão depois
  {
    diretorios[i] = diretorios[i + 1];
    diretorios[i]->ordem = diretorios[i]->ordem - 1;
  }

  free(diretorios[tam-1]);
  tam--;

  if (tam == 0)
    fim = 0;
  else 
    fim = diretorios[tam-1]->local + diretorios[tam-1]->tamanho_disc;

  ftruncate(fileno(archive), fim);
  escreve_diretorio(diretorios, tam, archive);

  return;
}

void opcao_c(FILE *archive, struct diretorio **diretorios)
{
  int tam, i;

  if (!archive || !diretorios)
    return;

  tam = le_diretorio(diretorios, archive);
  if (tam <= 0)
    printf("Sem arquivos no archive\n");

  for (i = 0; i < tam; i++)
  {
    printf("=========================================\n");
    printf("Nome: %s\n", diretorios[i]->nome);
    printf("UID: %d\n", diretorios[i]->UID);
    printf("Tam.Og: %ld\n", diretorios[i]->tamanho_og);
    printf("Tam.Disco: %ld\n", diretorios[i]->tamanho_disc);
    printf("Data: %ld\n", diretorios[i]->data);
    printf("Ordem: %d\n", diretorios[i]->ordem);
    printf("Local: %d\n", diretorios[i]->local);
    printf("=========================================\n");
  }

  return;
}