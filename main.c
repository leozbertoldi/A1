#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "func.h"

int main(int argc, char **argv)
{
  int option, flag, criado, tamanho, capacidade, feito, mover;
  FILE *archive;
  struct diretorio *arquivo, *target, **diretorios;
  char m_mover[1024];
  tamanho = 0;
  criado = 0;
  feito = 0;
  mover = 0;
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
          mover = 1;
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
      archive = fopen(argv[i], "r+b");
      if (!archive)
        archive = fopen("archive.vc", "w+b"); // cria do zero
      printf("ARCHIVE CRIADO\n");
      criado = 1;
    }
    else if (flag == 1 && criado == 1)
    {
      if ((option != 3 && option != 5) && !mover)
        arquivo = inicializa_arquivo(argv[i]);

      if (arquivo || option == 3 || option == 5 || mover)
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
            opcao_ic(arquivo, archive, diretorios);
            break;

          case 2:
            if (strlen(m_mover) == 0) //verifica se m_mover está vazio
            {
              strncpy(m_mover, argv[i], 1024);
              break;
            }
            target = inicializa_arquivo(argv[i]);
            printf("caso -m\n");
            if (target)
              opcao_m(m_mover, target->nome, archive, diretorios);
            else 
              opcao_m(m_mover, NULL, archive, diretorios);
            break;

          case 3:
            printf("caso -x\n");
            opcao_x(argv[i], archive, diretorios);
            break;

          case 4:
            printf("caso -r\n");
            opcao_r(arquivo, archive, diretorios);
            break;

          case 5:
            if (feito)
              break;
            printf("caso -c\n");
            opcao_c(archive, diretorios);
            feito = 1;
            break;
  
          default:
            break;
        }
      }
    }
  }
  if (option == 5 && !feito)
    opcao_c(archive, diretorios);
  printf("\n");
  printf("Fim!\n");
  if (criado)
  {
    tamanho = le_diretorio(diretorios, archive);
    fclose(archive);
  }
  //destroi_diretorios(diretorios, tamanho);

  return 0;
}
