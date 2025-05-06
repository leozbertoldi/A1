#ifndef FUNCOES
#define FUNCOES

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

void escreve_diretorio(struct diretorio diretorios[], int n, FILE *archive);

void opcao_c(struct diretorio *arquivo,  FILE *archive);

void opcao_ip(struct diretorio *arquivo,  FILE *archive);

void opcao_ic(struct diretorio *arquivo, FILE *archive);

void opcao_m(struct diretorio *arquivo, FILE *archive);

void opcao_x(char *arquivo, FILE *archive);

void opcao_r(struct diretorio *arquivo, FILE *archive);

#endif