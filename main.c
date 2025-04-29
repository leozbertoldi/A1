#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "func.h"

int main(int argc, char **argv)
{
  int option, ordem;
  long int local;
  struct diretorio *FILE;

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
      option = 1;
    else 
      option = 0;
    
    if (option)
    {
      switch (argv[i][1])
      {
        case 'i':
          if(argv[i][2] == 'p')
            printf("caso -ip\n");
          else if (argv[i][2] == 'c')
            printf("caso -ic\n");
          break;

        case 'm':
          printf("caso -m membro\n");
          break;

        case 'x':
          printf("caso -x\n");
          break;

        case 'r':
          printf("caso -r\n");
          break;

        case 'c':
          printf("caso -c\n");
          FILE = inicializa_arquivo(argv[i+1], i+1 /*ordem futura*/, i+1 /*local*/);
          if (FILE)
            opcao_c(FILE);
          break;

        default:
          break;
      }
    }
  }
  printf("\n");
  printf("Fim!\n");

  return 0;
}

//ftrunc 
//if tamanho >= compactado 
//if compactado > normal armazena normal (se der o mesmo tamanhop, ficar com o plano)
//nota extra pra permissões do arquivo
