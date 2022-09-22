#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int duplicado = 0;
int n = 4;
int **matrix;

void *lerLinha(int l_num)
{
  for (int i = 0; i < n - 1; ++i)
    for (int j = i + 1; j < n; ++j)
      if (matrix[l_num][j] == matrix[l_num][i])
      {
        duplicado = 1;
        pthread_exit(NULL);
      }

  pthread_exit(NULL);
}

void *lerColuna(int c_num)
{
  for (int i = 0; i < n - 1; i++)
    for (int j = i + 1; j < n; j++)
    {
      if (matrix[j][c_num] == matrix[i][c_num])
      {
        duplicado = 1;
        pthread_exit(NULL);
      }
    }
  pthread_exit(NULL);
}

int main()
{

  srand(time(NULL));
  printf("Tamanho da matrix?");
  scanf("%d", &n);
  matrix = malloc(sizeof(int *) * n);

  printf("Digite cada valor separado por espaco (numeros de 0-9)");
  for (int i = 0; i < n; i++)
  {
    matrix[i] = malloc(sizeof(int) * n);
    for (int j = 0; j < n; j++)
    {
      scanf("%d", &matrix[i][j]);
    }
  }

  for (size_t i = 0; i < n; i++)
  {
    printf("\n");
    for (size_t j = 0; j < n; j++)
      printf("%d ", matrix[i][j]);
  }
  printf("\n");

  pthread_t t_linhas[n];
  pthread_t t_colunas[n];

  for (size_t i = 0; i < n; i++)
    pthread_create(&t_linhas[i], NULL, (void *)lerLinha, (void *)i);

  for (size_t i = 0; i < n; i++)
    pthread_create(&t_colunas[i], NULL, (void *)lerColuna, (void *)i);

  for (size_t i = 0; i < n; i++)
    pthread_join(&t_linhas[i], NULL);

  for (size_t i = 0; i < n; i++)
    pthread_join(&t_colunas[i], NULL);

  if (duplicado == 1)
    printf("Matrix normal");
  else
    printf("Matrix latina");

  return 0;
}