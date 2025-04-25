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
          break;

        default:
          break;
      }
    }
    else 
    {
      FILE = inicializa_arquivo(argv[i], i /*ordem futura*/, i /*local*/, argv[i]);
      printf("Arquivo %s:\n", argv[i]);
      printf("Nome: %s\n", FILE->nome);
      printf("UID: %d\n", FILE->UID);
      printf("Tam.Og: %ld\n", FILE->tamanho_og);
      printf("Tam.Disco: %ld\n", FILE->tamanho_disc);
      printf("Data: %ld\n", FILE->data);
      printf("Ordem: %d\n", FILE->ordem);
      printf("Local: %d\n", FILE->local);
    }
  }

  return 0;
}

//ftrunc 
//if tamanho >= compactado 
//if compactado > normal armazena normal (se der o mesmo tamanhop, ficar com o plano)
//nota extra pra permissões do arquivo
