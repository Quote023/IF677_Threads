#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct somar_props
{
  int id;
  int qtd_candidatos;
  int *votos;
};

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void *somar(void *args)
{
  struct somar_props *props = (struct somar_props *)args;
  int x = 0;
  char nomeArquivo[6];
  sprintf(nomeArquivo, "%d.in", props->id);

  FILE *file = fopen(nomeArquivo, "r");
  while (!feof(file))
  {
    fscanf(file, "%d", &x);
    while (pthread_mutex_trylock(&mymutex))
      ;
    ++(props->votos[x]);
    pthread_mutex_unlock(&mymutex);
  }

  return 0;
}

int setup(int qtd_candidatos, int qtd_threads)
{
  pthread_t ts[qtd_threads];
  int votos[qtd_candidatos + 1];
  int total = 0;
  int vencedor = 0;

  memset(votos, 0, sizeof(votos));

  for (int i = 0; i < qtd_threads; ++i)
    pthread_create(&ts[i], NULL, somar, &(struct somar_props){.id = i, .qtd_candidatos = qtd_candidatos, .votos = votos});
  for (int i = 0; i < qtd_threads; ++i)
    pthread_join(ts[i], NULL);

  for (int i = 0; i <= qtd_candidatos; ++i)
    total += votos[i];

  for (int i = 1; i <= qtd_candidatos; ++i)
  {
    if (votos[vencedor] < votos[i])
      vencedor = i;
    printf("Candidato %d:\tVotos: %d\t(%d%%)\n", i, votos[i], ((votos[i] * 100) / total));
  }

  printf("\nVotos Nulos: %d\t(%d%%) \nTotal de Votos: %d\n", votos[0], ((votos[0] * 100) / total), total);
  printf("O Candidato %d venceu as eleicoes!\n", vencedor);

  return 0;
}

int main(int argc, char const *argv[])
{
  return setup(3, 3);
}