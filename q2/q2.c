#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_TRAIN 10 // quantidade de trens
#define NUM_INTER 5  // numero de intersecoes

void *inRoute(void *threadtrain)
{
    int trains = *((int *)threadtrain);

    for (int i = 0; i < NUM_TRAIN; i++)
    {
        int count_cicles = 0; // inicializacao da variavel numero de voltas de um trem
        int count_trains = 0; // inicializacao da variavel quantidade de trens na intersecao
        bool cond_var = true;

        for (int j = 1; j < (count_cicles + 1); j++)
        {                    // intersecoes
            while (cond_var) // enquanto puder entrar na intersecao
            {
                // mutex lock
                //  aqui usar o pthread_cond_wait(cond_var, mutex)
                count_trains = enterIntersection(i, j, *count_trains);
                if (count_trains < 2)
                {
                    cond_var = true;
                }
                else
                {
                    cond_var = false;
                }
                sleep(0.500);
                count_trains = outIntersection(i, j, *count_trains);
                
                if (count_trains < 2)
                {
                    cond_var = true;
                }
                else
                {
                    cond_var = false;
                }
                pthread_cond_broadcast(cond_var); // aqui tem que avisar só para o próximo trem
            }
        }

        count_cicles += count_cicles;
        count_cicles = count_cicles % NUM_INTER; // mod 5 para o loop dos trens seguindo o percurso das intersecoes
    }

    pthread_exit(NULL);
}

int enterIntersection(int const trem, int const inter, int const &arg)
{
    printf("trem #%d entra na interseção #%d \n", trem, inter);
    // a ideia aqui é receber o contador de trens na interseção, acrescer 1 e passar atualizado na função inRoute
    return &arg;
}

int outIntersection(int const trem, int const inter, int const &arg)
{
    printf("trem #%d sai da interseção #%d \n", trem, inter);
    // a ideia aqui é receber o contador de trens na interseção, descrescer 1 e passar atualizado na função inRoute
    return &arg;
}

int main()
{
    pthread_t threads[NUM_TRAIN];
    int *train[NUM_TRAIN];

    int rc;
    int t;
    for (t = 0; t < NUM_TRAIN; t++)
    {
        train[t] = (int *)malloc(sizeof(int));
        *train[t] = t;
        rc = pthread_create(&threads[t], NULL, inRoute, (void *)train[t]);
        if (rc)
        {
            printf("ERRO; código de retorno é %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL); // Desaloca as posicoes
}
