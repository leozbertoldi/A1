#ifndef FUNCOES
#define FUNCOES
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

struct diretorio
{
  char nome[1024];
  unsigned int UID;
  long int tamanho_og;
  long int tamanho_disc;
  long int data;
  int ordem;
  int local;
};

struct diretorio *inicializa_arquivo(char *arquivo);

int le_diretorio(struct diretorio **diretorios, FILE *archive);

void escreve_diretorio(struct diretorio **diretorios, int n, FILE *archive, int insere);

void destroi_diretorios(struct diretorio **diretorios, int n);

void opcao_c(FILE *archive, struct diretorio **diretorios);

void opcao_ip(struct diretorio *arquivo,  FILE *archive, struct diretorio **diretorios);

void opcao_ic(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios);

void opcao_m(char *arquivo, char *target, FILE *archive, struct diretorio **diretorios);

void opcao_x(char *arquivo, FILE *archive, struct diretorio **diretorios);

void opcao_r(struct diretorio *arquivo, FILE *archive, struct diretorio **diretorios);

#endif
