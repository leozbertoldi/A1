#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "func.h"

struct diretorio *inicializa_arquivo(char *arquivo, int ordem, long int local)
{
  struct stat info;
  struct diretorio *d = malloc(sizeof(struct diretorio));

  if (stat(arquivo, &info) != 0) 
  {
    printf("Erro ao acessar o arquivo\n");
    free(d);
    return NULL;
  }

  strncpy(d->nome, arquivo, 99);
  d->UID = info.st_uid;
  d->tamanho_og = info.st_size;
  d->tamanho_disc = info.st_size * 512;
  d->data = info.st_mtime; 
  d->ordem = ordem;
  d->local = local;

  return d;
}

void opcao_ip(struct diretorio *arquivo)
{
  struct diretorio *FILE;
  //fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
  return;
}

void opcao_m(struct diretorio *arquivo)
{
  return;
}

void opcao_x(struct diretorio *arquivo)
{
  return;
}

void opcao_r(struct diretorio *arquivo)
{
  return;
}

void opcao_c(struct diretorio *arquivo)
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
