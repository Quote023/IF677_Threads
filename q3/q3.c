#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ESCRITORAS_COUNT 2
#define LEITORAS_COUNT 3

#define INDISPONIVEL 0
#define DISPONIVEL 1

#define W_DESCANSO 0
#define W_SOLICITANDO 1

pthread_mutex_t mtx_escr = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_rstatus = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_wstatus = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_pode_ler = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pode_escrever = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx_rcount = PTHREAD_MUTEX_INITIALIZER;

char r_status = INDISPONIVEL;
char w_status = W_DESCANSO;

int r_count = 0;

char letras[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'n', 'm', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

void *reader(int n)
{
  srand(time(NULL));

  while (1)
  {
    pthread_mutex_lock(&mtx_wstatus); // Caso uma Thread de Escrita esteja solicitando a vez para escrever
    while (w_status == W_SOLICITANDO)
    {
      // Trava todas as threads de leitura e espera a escrita ser realizada
      pthread_cond_wait(&cond_pode_ler, &mtx_wstatus);
    }
    pthread_mutex_unlock(&mtx_wstatus); // Caso nenhuma Thread de escrita solicite a vez, vai dar lock e unlock logo depois

    pthread_mutex_lock(&mtx_rcount);
    ++r_count;
    pthread_mutex_unlock(&mtx_rcount);

    int p1 = rand() % 26;
    char valor = letras[p1];
    sleep(1);
    pthread_mutex_lock(&mtx_rcount);
    printf("Leitora %d realizou uma leitura e retornou o valor %c! Restam: %d\n", n, valor, r_count);
    if (--r_count == 0)
    {
      printf("Todas as Leituras concluidas, escrita liberada!\n");
      pthread_cond_broadcast(&cond_pode_escrever);
    }
    pthread_mutex_unlock(&mtx_rcount);
  }
  pthread_exit(NULL);
}

void *escrever(int n)
{
  while (1)
  {
    sleep(2);
    pthread_mutex_lock(&mtx_escr); // Trava todas as threads de escrita
    w_status = W_SOLICITANDO;
    printf("Escritora %d está aguardando para escrever!\n", n);

    pthread_mutex_lock(&mtx_rcount);
    while (r_count > 0)
      pthread_cond_wait(&cond_pode_escrever, &mtx_rcount);
    pthread_mutex_unlock(&mtx_rcount);

    printf("Escritora %d iniciando operacao de escrita\n", n);

    int p1 = rand() % 26;
    int p2 = rand() % 26;
    char tmp = letras[p1];
    letras[p1] = letras[p2];
    letras[p2] = tmp;

    printf("Escritora %d realizou uma escrita!\n", n);
    w_status = W_DESCANSO;
    pthread_cond_broadcast(&cond_pode_ler);
    pthread_mutex_unlock(&mtx_escr);
  }
  pthread_exit(NULL);
}

int main()
{
  pthread_t threads_l[LEITORAS_COUNT];
  pthread_t threads_e[ESCRITORAS_COUNT];

  for (size_t i = 0; i < LEITORAS_COUNT; i++)
    pthread_create(&threads_l[i], NULL, (void *)reader, (void *)i);

  for (size_t i = 0; i < ESCRITORAS_COUNT; i++)
    pthread_create(&threads_e[i], NULL, (void *)escrever, (void *)i);

  for (size_t i = 0; i < ESCRITORAS_COUNT; i++)
    pthread_join(threads_e[i], NULL);
  for (size_t i = 0; i < ESCRITORAS_COUNT; i++)
    pthread_join(threads_l[i], NULL);

  pthread_exit(NULL);
  return 0;
}