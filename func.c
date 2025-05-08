#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "func.h"

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
  int tam, i;

  if (!diretorios || !archive)
    return -1;

  if (ftell(archive) == 0)
    return 0;

  fseek(archive, -sizeof(int), SEEK_END); //achar o tamanho do diretório
  fread(&tam, sizeof(int), 1, archive); //pega o valor e armazena em tam
  if (tam != 1) //se deu errado a leitura
    return 0;

  fseek(archive, -(sizeof(int) + tam * sizeof(struct diretorio)), SEEK_END); //vai até o começo do diretório
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

void opcao_ip(struct diretorio *arquivo, FILE *archive)
{
  FILE *file;
  long int bytes, offset;
  char *buffer;

  if (!arquivo || !archive)
    return;
  
  //if (not repetido)

  buffer = malloc(arquivo->tamanho_og);
  if (!buffer)
    return;

  file = fopen(arquivo->nome, "rb+");
  if (!file)
  {
    printf("Erro ao abrir o arquivo %s\n", arquivo->nome);
  }

  fseek(archive, 0, SEEK_END);
  offset = ftell(archive); //ftell serve para falar a posição do ponteiro

  bytes = fread(buffer, 1, sizeof(buffer), file);
  while (bytes > 0)
  {
    fwrite(buffer, 1, bytes, archive);
    bytes = fread(buffer, 1, sizeof(buffer), file);
  }

  fclose(file);
  arquivo->ordem = 0;
  arquivo->local = offset;
  arquivo->tamanho_disc = arquivo->tamanho_og;
  free(buffer);

  return;
}

/*void opcao_ic(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios)
{

}

void opcao_m(struct diretorio *arquivo, FILE  *archive, struct diretorio **diretorios)
{
  return;
}*/

void opcao_x(struct diretorio *arquivo, FILE *archive)
{
  FILE *file;
  char buffer[1024];
  long int bytes;

  if (!arquivo || !archive)
    return;

  file = fopen(arquivo->nome, "wb");
  if (!file)
  {
    printf("Erro ao abrir o arquivo %s\n", arquivo->nome);
    return;
  }

  fseek(archive, arquivo->local, SEEK_SET);
  bytes = fread(buffer, 1, sizeof(buffer), archive);
  while (bytes > 0)
  {
    fwrite(buffer, 1, bytes, file);
    bytes = fread(buffer, 1, sizeof(buffer), archive);
  }

  fclose(file);

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
