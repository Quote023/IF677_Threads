#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int check = 0;
int n, **matrix;

void *lerLinha()
{
  for (int i = 0; i < n; i++)
  {
    int a = matrix[0][i];
    for (int j = i + 1; j < n; j++)
    {
      if (matrix[i][j] == a)
      {
        check = 1;
        pthread_exit(NULL);
      }
    }
  }
  pthread_exit(NULL);
}

void *lerColuna()
{
  for (int i = 0; i < n; i++)
  {
    int b = matrix[i][0];
    for (int j = i + 1; j < n; j++)
    {
      if (matrix[j][i] == b)
      {
        check = 1;
        pthread_exit(NULL);
      }
    }
  }
  pthread_exit(NULL);
}

int main()
{
  pthread_t thread1, thread2;

  printf("Tamanho da matrix?");
  scanf("%d", &n);

  matrix = malloc(sizeof(int *) * n);

  for (int i = 0; i < n; i++)
  {
    matrix[i] = malloc(sizeof(int) * n);
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      matrix[i][j] = (i * 10) + j;
    }
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      matrix[i][j] = rand() % 10;
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }

  pthread_create(&thread1, NULL, lerLinha, NULL);
  pthread_create(&thread2, NULL, lerColuna, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  if (check == 1)
  {
    printf("Matrix normal");
  }
  else
  {
    printf("Matrix latina");
  }
  return 0;
}