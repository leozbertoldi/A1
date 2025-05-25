#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "func.h"

int main(int argc, char **argv)
{
  int option, capacidade, tamanho;
  FILE *archive;
  struct diretorio *arquivo, *target, **diretorios;
  char m_mover[1024];
  capacidade = 50;

  diretorios = malloc(capacidade * sizeof(struct diretorio *));
  if (!diretorios)
    return -1;

  if (argc < 3 || argv[1][0] != '-')
    return;

  archive = fopen(argv[2], "r+b");
  if (!archive)
  {
    archive = fopen(argv[2], "w+b"); // cria do zero
    printf("ARCHIVE CRIADO\n");
  }

  tamanho = le_diretorio(diretorios, archive);

  switch (argv[1][1])
  {
    case 'i':
      arquivo = inicializa_arquivo(argv[3]);
      if (!arquivo)
      {
        fclose(archive);
        destroi_diretorios(diretorios, tamanho);
        diretorios = NULL;
      }
      if(argv[1][2] == 'p')
      {
        printf("caso -ip\n");
        opcao_ip(arquivo, archive, diretorios);
      }
      else if (argv[1][2] == 'c')
      {
        printf("caso -ic\n");
        opcao_ic(arquivo, archive, diretorios);
      }
      break;

    case 'm':
      if (argc == 4)
        opcao_m(argv[3], diretorios[0]->nome, archive, diretorios);
      else 
        opcao_m(argv[3], argv[4], archive, diretorios);
      break;

    case 'x':
      printf("caso -x\n");
      if (argc == 4)
        opcao_x(argv[3], archive, diretorios);
      else
        opcao_x(NULL, archive, diretorios);
      break;

    case 'r':
      printf("caso -r\n");
      arquivo = inicializa_arquivo(argv[3]);
      if (!arquivo)
      {
        fclose(archive);
        destroi_diretorios(diretorios, tamanho);
        diretorios = NULL;
      }
      opcao_r(arquivo, archive, diretorios);
      break;

    case 'c':
      printf("caso -c\n");
      opcao_c(archive, diretorios);
      break;

    default:
       break;
  }

  tamanho = le_diretorio(diretorios, archive); //atualiza tamanho depois das alterações
  fclose(archive);
  destroi_diretorios(diretorios, tamanho);
  diretorios = NULL;
  printf("\n");
  printf("Fim!\n");
  return 0;
}
