#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_TRAIN 10 // quantidade de trens
#define NUM_INTER 5  // numero de intersecoes
#define TRENS_POR_INTERSEC 2

pthread_mutex_t mtx_intersec[NUM_INTER];
pthread_cond_t cond_interesc[NUM_INTER];

pthread_mutex_t mtx_count[NUM_INTER];
int train_count[NUM_INTER] = {0};

void *inRoute(int id)
{
  int c_inter = 0;

  while (1)
  {
    // entrar na interseccão
    pthread_mutex_lock(&mtx_intersec[c_inter]);
    pthread_mutex_lock(&mtx_count[c_inter]);
    printf("trem %d tentando entrar na interseccao %d\n", id, c_inter + 1);
    while (train_count[c_inter] >= TRENS_POR_INTERSEC)
    {
      printf("trem %d esperando interseccao %d ser liberada\n", id, c_inter + 1);
      // caso já tenha 2 trens na intersecção
      // espera a cond_var liberar
      pthread_cond_wait(&cond_interesc[c_inter], &mtx_count[c_inter]);
    }
    pthread_mutex_unlock(&mtx_count[c_inter]);
    // se tiver menos que 2 trens: entra na intersecção e libera o mutex pro proximo
    pthread_mutex_unlock(&mtx_intersec[c_inter]);

    pthread_mutex_lock(&mtx_count[c_inter]);
    ++train_count[c_inter]; // aumenta a quantidade de trens
    pthread_mutex_unlock(&mtx_count[c_inter]);
    printf("trem %d passando pela interseccao %d\n", id, c_inter + 1);
    usleep(500); // espera

    pthread_mutex_lock(&mtx_count[c_inter]);
    --train_count[c_inter]; // diminui a quantidade de trens
    pthread_mutex_unlock(&mtx_count[c_inter]);
    printf("trem %d saiu da interseccao %d\n", id, c_inter + 1);

    pthread_cond_broadcast(&cond_interesc[c_inter]); // libera pro próximo
    c_inter = (c_inter + 1) % 5;
  }

  pthread_exit(NULL);
}

int main()
{
  pthread_t threads[NUM_TRAIN];

  for (size_t i = 0; i < NUM_INTER; i++)
  { // Inicializar mutexes e condicionais
    pthread_mutex_init(&mtx_intersec[i], NULL);
    pthread_mutex_init(&mtx_count[i], NULL);
    pthread_cond_init(&cond_interesc[i], NULL);
  }

  for (size_t t = 0; t < NUM_TRAIN; t++)
  {
    int rc = pthread_create(&threads[t], NULL, (void *)inRoute, (void *)t);
    if (rc)
    {
      printf("ERRO; código de retorno é %d\n", rc);
      exit(-1);
    }
  }

  for (size_t i = 0; i < NUM_TRAIN; i++)
    pthread_join(threads[i], NULL);

  pthread_exit(NULL); // Desaloca as posicoes
}
