#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "func.h"

int main(int argc, char **argv)
{
  int option, flag, criado, tamanho, capacidade;
  FILE *archive;
  struct diretorio *arquivo, **diretorios;
  tamanho = 0;
  criado = 0;
  capacidade = 10;

  diretorios = malloc(capacidade * sizeof(struct diretorio *));
  if (!diretorios)
    return -1;

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
        case 'i':
          if(argv[i][2] == 'p')
          {
            option = 0;
            flag = 1;
          }
          else if (argv[i][2] == 'c')
          {
            option = 1;
            flag = 1;
          }
          break;

        case 'm':
          option = 2;
          flag = 1;
          break;

        case 'x':
          option = 3;
          flag = 1;
          break;

        case 'r':
          option = 4;
          flag = 1;
          break;

        case 'c':
          option = 5;
          flag = 1;
          break;

        default:
          break;
      }
    }  
    else if (strcmp(argv[i], "archive.vc") == 0 && criado == 0)
    {
      archive = fopen(argv[i], "ab+");
      printf("ARCHIVE CRIADO\n");
      criado = 1;
    }
    else if (flag == 1 && criado == 1)
    {
      if (option != 3)
        arquivo = inicializa_arquivo(argv[i]);

      if (arquivo || option == 3)
      {
        switch (option)
        {
          case 0:
            printf("caso -ip\n");
            tamanho = le_diretorio(diretorios, archive);
            if (tamanho == capacidade)
            {
              capacidade *= 2; //dobra a capacidade
              diretorios = realloc(diretorios, capacidade * sizeof(struct diretorio *));
              if (!diretorios)
              {
                printf("Erro ao realocar vetor de diretórios\n");
                return -1;
              }
            }          
            opcao_ip(arquivo, archive, diretorios);
            break;

          case 1:
            printf("caso -ic\n");
            break;

          case 2:
            printf("caso -m\n");
            break;

          case 3:
            printf("caso -x\n");
            opcao_x(arquivo, archive);
            break;

          case 4:
            printf("caso -r\n");
            break;

          case 5:
            opcao_c(archive, diretorios);
            break;
  
          default:
            break;
        }
      }
    }
  }
  printf("\n");
  printf("Fim!\n");
  if (criado)
    fclose(archive);

  return 0;
}

//if compactado > normal armazena normal (se der o mesmo tamanhop, ficar com o plano)