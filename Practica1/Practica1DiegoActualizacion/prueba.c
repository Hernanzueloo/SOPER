#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pow.h"

#define MAX POW_LIMIT - 1
#define MAX_HILOS 2

typedef struct
{
    long ep, eu, res;
} entradaHash;
int a=0;

void *func_minero(void *arg);

long minero(int nHilos, long busq);

int main()
{
    int i, rc[MAX_HILOS],busq=10;
    entradaHash t[MAX_HILOS];
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];


    a=0;
    for (i = 0; i < 5; i++)
    {
        t[i].ep = ((long)100 / 5) * i;
        t[i].eu = ((long)100 / 5) * (i + 1);
        t[i].res = busq;
        rc[i] = pthread_create(&threads[i], NULL, func_minero, (void *)&t[i]);
        if (rc[i])
        {
            printf("Error creando el hilo %d", i);
            return 1;
        }
    }
    for (i = 0; i < 5; i++)
    {
        rc[i] = pthread_join(threads[i], &sol[i]);
        if (rc[i])
        {
            printf("Error joining thread %d\n", i);
            return -1;
        }
    }
        for (i = 0; i < 5; i++){
            printf("%d",sol[i]);
        }
    return 0;

}

void *func_minero(void *arg)
{
    int i;
    long x = -1;
    entradaHash *e = (entradaHash *)arg;

    for (i = e->ep; (i < e->eu) && (x <= 0) && (a == 0); i++)
    {
        if ((long)i == e->res)
        {
            x = i;
            a=1;
        }
    }

    pthread_exit((void *)x);
}

long minero(int nHilos, long busq)
{
    int i, rc[MAX_HILOS];
    entradaHash t[MAX_HILOS];
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];


    
    /*Creacion de los hilos*/
    for (i = 0; i < nHilos; i++)
    {
        t[i].ep = ((long)MAX / nHilos) * i;
        t[i].eu = ((long)MAX / nHilos) * (i + 1);
        t[i].res = busq;
        rc[i] = pthread_create(&threads[i], NULL, func_minero, (void *)&t[i]);
        if (rc[i])
        {
            printf("Error creando el hilo %d", i);
            return 1;
        }
    }

    /*Joins de los hilos*/
    for (i = 0; i < nHilos; i++)
    {
        rc[i] = pthread_join(threads[i], &sol[i]);
        if (rc[i])
        {
            printf("Error joining thread %d\n", i);
            return -1;
        }
    }

    for (i = 0; i < nHilos; i++)
    {
        if ((long)sol[i] != -1)
        {
            return (long)sol[i];
        }
    }
}