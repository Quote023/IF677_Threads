#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FILOSOFOS_COUNT 5

pthread_t filosofos[FILOSOFOS_COUNT];
pthread_mutex_t garfos[FILOSOFOS_COUNT];

int think(int n)
{
  printf("\nFilosofo %d esta pensando...", n);
  return 0;
}

int get_forks(int n)
{
  pthread_mutex_lock(&garfos[n]);
  pthread_mutex_lock(&garfos[(n + 1) % 5]);
  return 0;
}

int eat(int n)
{
  printf("\nFilosofo %d esta comendo!", n);
  sleep(n + 1);
  return 0;
}

int put_forks(int n)
{
  pthread_mutex_unlock(&garfos[n]);
  pthread_mutex_unlock(&garfos[(n + 1) % 5]);
  return 0;
}

void *filosofo(int *n)
{
  while (1)
  {
    think(*n);
    get_forks(*n);
    eat(*n);
    put_forks(*n);
    printf("\nFilosofo %d terminou de comer!", *n);
  }

  return 0;
}

int main(int argc, char const *argv[])
{
  // Inicializar mutexes
  for (size_t i = 0; i < FILOSOFOS_COUNT; i++)
    pthread_mutex_init(&garfos[i], NULL);

  // Inicializar threads
  for (size_t i = 0; i < FILOSOFOS_COUNT; i++)
    pthread_create(&filosofos[i], NULL, (void *)filosofo, (void *)i);

  // Aguarda finalizar
  for (size_t i = 0; i < FILOSOFOS_COUNT; i++)
    pthread_join(filosofos[i], NULL);

  pthread_exit(NULL);
  return 0;
}
