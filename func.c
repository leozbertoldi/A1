#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "func.h"

struct diretorio *inicializa_arquivo(char *arquivo, int ordem, long int local, char *argv)
{
  struct stat info;
  struct diretorio *d = malloc(sizeof(struct diretorio));

  if (stat(arquivo, &info) != 0) 
  {
    printf("Erro ao acessar o arquivo\n");
    free(d);
    return NULL;
  }

  strncpy(d->nome, argv, 99);
  d->UID = info.st_uid;
  d->tamanho_og = info.st_size;
  d->tamanho_disc = info.st_size * 512;
  d->data = info.st_mtime; 
  d->ordem = ordem;
  d->local = local;

  return d;
}
