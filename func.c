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

void escreve_diretorio(struct diretorio diretorios[], int n, FILE *archive)
{
  if (!diretorios || !archive)
    return;

  for (int i = 0; i < n; i++)
    fwrite(&diretorios[i], sizeof(struct diretorio), 1, archive);

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

/*void opcao_ic(struct diretorio *arquivo, FILE *archive)
{

}

void opcao_m(struct diretorio *arquivo, FILE  *archive)
{
  return;
}*/

void opcao_x(char *arquivo, FILE *archive)
{
  FILE *file;
  char buffer[1024];
  long int bytes;

  if (!arquivo || !archive)
    return;

  file = fopen(arquivo, "wb");
  if (!file)
  {
    printf("Erro ao abrir o arquivo %s\n", arquivo);
    return;
  }

  fseek(archive, 0/*arquivo->local*/, SEEK_SET);
  bytes = fread(buffer, 1, sizeof(buffer), archive);
  while (bytes > 0)
  {
    fwrite(buffer, 1, bytes, file);
    bytes = fread(buffer, 1, sizeof(buffer), archive);
  }

  fclose(file);

  return;
}

/*void opcao_r(struct diretorio *arquivo, FILE *archive)
{
  return;
}*/

void opcao_c(struct diretorio *arquivo, FILE *archive)
{
  printf("=========================================\n");
  printf("Nome: %s\n", arquivo->nome);
  printf("UID: %d\n", arquivo->UID);
  printf("Tam.Og: %ld\n", arquivo->tamanho_og);
  printf("Tam.Disco: %ld\n", arquivo->tamanho_disc);
  printf("Data: %ld\n", arquivo->data);
  printf("Ordem: %d\n", arquivo->ordem);
  printf("Local: %d\n", arquivo->local);
  printf("=========================================\n");

  return;
}
