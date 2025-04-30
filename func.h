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

struct diretorio *inicializa_arquivo(char *arquivo, int ordem, long int local);

void opcao_c(struct diretorio *arquivo, struct diretorio *archive);

void opcao_ip(struct diretorio *arquivo, struct diretorio *archive);

void opcao_m(struct diretorio *arquivo, struct diretorio *archive);

void opcao_x(struct diretorio *arquivo, struct diretorio *archive);

void opcao_r(struct diretorio *arquivo, struct diretorio *archive);

#endif