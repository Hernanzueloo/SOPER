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
            printf("\nSolution rejected: %08ld !-> %08ld, %d", busq, solucion, i);
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
    int i, rc[MAX_HILOS],incr;
    entradaHash t[MAX_HILOS];
    pthread_t threads[MAX_HILOS];
    int *sol[MAX_HILOS];
    int solucion=-1;

    printf("Buscamos: %d, Hilos: %d\n",busq, nHilos);
    
    incr=((int)MAX) / nHilos;
    printf("%d\n",incr);
    /*Creacion de los hilos*/
    for (i = 0; i < nHilos; i++)
    {
        t[i].ep = incr * i;
         
        t[i].eu = incr * (i+1);
        
        if( i == nHilos-1){
            printf("Cambios:%d\n", (incr * i)<MAX);

            t[i].eu = MAX;
        }
        printf("Ini: %d Fin: %d\n",t[i].ep, t[i].eu);
        t[i].res = busq;
        rc[i] = pthread_create(&threads[i], NULL, func_minero, (void *)(t+i));
        if (rc[i])
        {
            printf("Error creando el hilo %d", i);
            return 1;
        }
    }

    /*Joins de los hilos*/
    for (i = 0; i < nHilos; i++)
    {
        rc[i] = pthread_join(threads[i], sol+i);
        if (rc[i])
        {
            printf("Error joining thread %d\n", i);
            return -1;
        }
    }
    
   
    for (i = 0; i < nHilos; i++)
    {
        if (sol[i] != -1)
        {
            solucion=(int)sol[i];
            printf("\nI: %d S:%ld\n",i,solucion);
           
            
        }
        
    }
    printf("Solucion: %d Buscamos: %d\n",solucion, busq);
    return solucion;
}