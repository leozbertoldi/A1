#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "func.h"

struct diretorio *inicializa_arquivo(char *arquivo)
{
  struct stat info;
  struct diretorio *d = malloc(sizeof(struct diretorio));

  if (stat(arquivo, &info) != 0) 
  {
    printf("Erro ao acessar o arquivo");
    return NULL;
  }

  d->UID = info.st_uid;
  d->tamanho_og = info.st_size;
  d->tamanho_disc = info.st_size * 512;
  d->data = 

}
