#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
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

  tam_archive = ftell(archive);
  if (tam_archive < (long int)sizeof(int))
    return 0;

  fseek(archive, -sizeof(int), SEEK_END); //achar o tamanho do diretório
  fread(&tam, sizeof(int), 1, archive); //pega o valor e armazena em tam
  if (tam != 1) //se deu errado a leitura
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
  if (!diretorios || !archive)
    return;

  for (int i = 0; i < n; i++)
    fwrite(diretorios[i], sizeof(struct diretorio), 1, archive);

  fwrite(&n, sizeof(int), 1, archive);

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

  file = fopen(arquivo->nome, "rb+");
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
  long int bytes, offset, og_size, new_size;
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

  file = fopen(arquivo->nome, "rb+");
  if (!file)
    printf("Erro ao abrir o arquivo %s\n", arquivo->nome);

  fseek(file, 0, SEEK_END);
  og_size = ftell(file);
  conteudo = malloc(sizeof(og_size));
  if (!conteudo)
  {
    printf("Erro ao alocar conteudo de arquivo para comprimir\n");
    fclose(file);
    return;
  }
  new_conteudo = malloc(2 * sizeof(og_size)); //a compressão pode ser maior
  if (!new_conteudo)
  {
    printf("Erro ao alocar conteudo de arquivo para comprimir\n");
    fclose(file);
    return;
  }
  fseek(file, 0, SEEK_SET);
  fread(conteudo, 1, og_size, file);
  new_size = LZ_Compress((unsigned char *)conteudo,(unsigned char *)new_conteudo, og_size);
  new = fopen(new_conteudo, "wb"); // contruindo o arquivo comprimido em new

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
  fclose(new);
  fclose(file);
  free(conteudo);
  free(new_conteudo);
  return;
}

/*void opcao_m(struct diretorio *arquivo, FILE  *archive, struct diretorio **diretorios)
{
  return;
}*/

void opcao_x(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{
  FILE *file;
  char buffer[1024];
  long int bytes;
  int num, i, tam;

  if (!archive)
    return;

  tam = le_diretorio(diretorios, archive); //lê o diretório em archive
  if (tam < 0)
    printf("Erro na leitura do diretório\n");

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
  
    file = fopen(arquivo->nome, "wb");
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

/*void opcao_r(struct diretorio *arquivo, FILE *archive, struct diretorio *diretorios[])
{
  return;
}*/

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
