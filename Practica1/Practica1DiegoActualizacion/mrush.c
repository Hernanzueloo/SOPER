#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pow.h"

#define MAX POW_LIMIT - 1
#define MAX_HILOS 10

typedef struct
{
    long ep, eu, res;
} entradaHash;

void *func_minero(void *arg);

long minero(int nHilos, long busq);

int main(int argc, char *argv[])
{
    int rc[MAX_HILOS], t1, t2, i;
    long solucion, busq;

    /*Control de errores*/
    t1 = clock();
    if ((atoi(argv[1]) < 0) || (atoi(argv[1]) > POW_LIMIT) || (atoi(argv[2]) < 0) || (atoi(argv[3]) < 0) || (atoi(argv[3]) > MAX_HILOS))
    {
        printf("\n\nError en los parametros de entrada");
        return 1;
    }

    /*Buscamos los elementos*/
    busq = atol(argv[1]);
    for (i = 0; i < atoi(argv[2]); i++)
    {
        solucion = minero(atoi(argv[3]), busq);

        /*Impresion de la solucion*/
        if (pow_hash(solucion) == busq)
        {
            printf("\nSolution accepted: %08ld --> %08ld", busq, solucion);
        }
        else
        {
            printf("\nSolution rejected: %08ld !-> %08ld", busq, solucion);
            printf("\nThe soluction has been invalidated");
            printf("\nMonitor exited with status 0");
            printf("\nMiner exited with status 0");
            return 1;
        }
        busq = solucion;
    }

    t2 = clock();
    printf("\nTime spent %d\n", t2 - t1);
    printf("\nMonitor exited with status 0");
    printf("\nMiner exited with status 0");

    return 0;
}

void *func_minero(void *arg)
{
    int i;
    long x = -1;
    entradaHash *e = (entradaHash *)arg;

    for (i = e->ep; (i < e->eu) && (x <= 0); i++)
    {
        if ((long)pow_hash(i) == e->res)
        {
            x = i;
        }
    }

    pthread_exit((void *)x);
}

long minero(int nHilos, long busq)
{
    int i, rc[MAX_HILOS], status, pipeCH[2], nbytes, parSol[2];
    entradaHash t[MAX_HILOS];
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];
    pid_t childpid;

    /*Creacion del fork monitor-terminal*/
    status = pipe(pipeCH);
    if (status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    childpid = fork(); /*0 si es el hijo y pid del hijo en el caso del padre*/

    if (childpid == -1)
    { /*Control de errores*/
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (childpid == 0) /*Caso hijo (terminal)*/
    {
        /*Cierre de lectura al final de en el hijo */
        close(fd[0]);
        do
        {
            nbytes = read(fd[0], &parSol, sizeof(int)*2);
            if (nbytes == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
        } while (nbytes != 0);
        
        if(pow_hash(parSol[0])==parSol[1]){
            printf("\nSolution accepted: %08ld --> %08ld", parSol[1], parSol[0]);
        }
        else{
            printf("\nSolution rejected: %08ld !-> %08ld", parSol[1], parSol[0]);
            printf("\nThe soluction has been invalidated");
            exit(EXIT_FAILURE);
        }
    }
    else
    { /*Caso padre (minero)*/
    }

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