#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include "pow.h"

#define MAX POW_LIMIT - 1
#define MAX_HILOS 10

typedef struct
{
    long ep, eu, res;
} entradaHash;

void *func_minero(void *arg);

void minero(int nHilos, long busq, int rondas);
int gestHilos(int nHilos, long busq, long *res);

int main(int argc, char *argv[])
{
    int rc[MAX_HILOS], t1, t2, i, status;
    pid_t childpid;

    /*Control de errores*/
    t1 = clock();
    if ((atoi(argv[1]) < 0) || (atoi(argv[1]) > POW_LIMIT) || (atoi(argv[2]) < 0) || (atoi(argv[3]) < 0) || (atoi(argv[3]) > MAX_HILOS))
    {
        printf("\n\nError en los parametros de entrada");
        return 1;
    }

    childpid = fork();
    if (childpid == 0)
    {
        minero(atoi(argv[3]), atol(argv[1]), (atoi(argv[2])));
    }
    else
    {
        wait(&status);
        /*Control de errores*/
        if (status == EXIT_SUCCESS)
        {
            printf("\nMiner exited with status 0");
        }
        else
        {
            printf("\nMiner exited with status 1");
        }

        t2 = clock();
        printf("\nTime spent %d\n", t2 - t1);
    }

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

void minero(int nHilos, long busq, int rondas)
{
    int i, j, rc[MAX_HILOS], pipe1[2], pipe2[2], nbytes, exitV;
    entradaHash t[MAX_HILOS];
    long parPH[2], comp[2] = {0, 0};
    pthread_t threads[MAX_HILOS];
    pid_t childpid;

    /*Creacion de las dos pipes de comunicacion*/
    if (pipe(pipe1) == -1)
    {
        perror("pipe1");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2) == -1)
    {
        perror("pipe2");
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
        close(pipe1[1]);
        close(pipe2[0]);

        for (j = 0; j < rondas; j++)
        {
            do
            {
                nbytes = read(pipe1[0], parPH, sizeof(long) * 2);
                if (nbytes == -1)
                {
                    printf("Error");
                    exit(EXIT_FAILURE);
                }
            } while (nbytes == 0);

            /*Impresion de la solucion*/
            if (pow_hash(parPH[1]) == parPH[0])
            {
                printf("\nSolution accepted: %08ld --> %08ld", parPH[0], parPH[1]);
                nbytes = write(pipe2[1], parPH, sizeof(long) * 2);
            }
            else
            {
                printf("\nSolution rejected: %08ld !-> %08ld", parPH[0], parPH[1]);
                printf("\nThe solution has been invalidated");
                parPH[0] = -1;
                parPH[1] = -1;
                nbytes = write(pipe2[1], parPH, sizeof(long) * 2);
                exit(EXIT_FAILURE);
            }
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        close(pipe1[0]);
        close(pipe2[1]);

        for (j = 0; j < rondas; j++)
        {
            if(gestHilos(nHilos, busq, parPH)==EXIT_FAILURE){
                printf("Error en la gestion de hilos\n");
                exit(EXIT_FAILURE);
            }

            nbytes = write(pipe1[1], parPH, sizeof(long) * 2);
            if(nbytes==-1){
                exit(EXIT_FAILURE);
            }

            do
            {
                nbytes = read(pipe2[0], comp, sizeof(long) * 2);
                if ((nbytes == -1) || comp[0] == -1)
                {
                    printf("Error leyendo pipe2");
                    exit(EXIT_FAILURE);
                }
            } while (comp[0] != parPH[0]);
            busq = parPH[1];
        }
    }

    wait(&exitV); // espera a que el proceso hijo termine

    if (exitV == EXIT_FAILURE)
    {
        printf("\nMonitor exited with status 1");
        exit(EXIT_FAILURE);
    }

    printf("\nMonitor exited with status 0");
    exit(EXIT_SUCCESS);
}

int gestHilos(int nHilos, long busq, long *res){
    int i, rc[MAX_HILOS], parPH[2];
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
                    exit(EXIT_FAILURE);
                }
            }

            /*Joins de los hilos*/
            for (i = 0; i < nHilos; i++)
            {
                rc[i] = pthread_join(threads[i], &sol[i]);
                if (rc[i])
                {
                    printf("Error joining thread %d\n", i);
                    exit(EXIT_FAILURE);
                }
            }

            /*Almacenar la solucion de los hilos*/
            for (i = 0; i < nHilos; i++)
            {
                if ((long)sol[i] != -1)
                {
                    res[0] = (long)busq;
                    res[1] = (long)sol[i];
                    i = nHilos; /*Para salir del bucle*/
                }
            }
    return EXIT_SUCCESS;
}