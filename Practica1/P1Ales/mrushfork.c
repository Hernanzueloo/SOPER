#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pow.h"

#define MAX POW_LIMIT-1
#define MAX_HILOS 10

typedef struct
{
    long ep, eu, res;
} entradaHash;

void *func_minero(void *arg);

int monitor(int pipeLectura, int pipeEscritura);

long minero(int nHilos,long nbusquedas, long busq);


long minar(int nHilos,long nbusquedas, long busq, int pipeLectura,int pipeEscritura);
int main(int argc, char *argv[])
{
    int rc[MAX_HILOS], t1, t2, i,newpid;
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
    newpid=fork();
    if(newpid){
            wait(); /*Aqui va el código que realiza el padre*/
    }else{
       
        busq = minero(atoi(argv[3]),atoi(argv[2]), busq); /*Queremos que minero devuelve el proximo numero a buscar*/
            
            
            /*Esto no hace falta ponerlo ya que esta tarea la realiza el proceso monitor*/

            /* 
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
            busq=solucion;*/
    
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

long minero(int nHilos, long nbusquedas, long busq)
{
    int newpid,status, pipeMon_min[2], pipeMin_mon[2];
    /*Creacion del proceso monitor*/
    status = pipe(pipeMon_min);
    if (status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    status = pipe(pipeMin_mon);
    if (status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    newpid=fork();
    if(newpid){
        /*Funcion que crea todos los hilos*/
        close(pipeMon_min[1]);
        close(pipeMin_mon[0]);
        if(minar(nHilos,nbusquedas,busq,pipeMon_min[0],pipeMin_mon[1])==0){
            return -1;
        }
        
    }else{
        /*Código del hijo, monitor*/
        close(pipeMin_mon[1]);
        close(pipeMon_min[0]);
        monitor(pipeMin_mon[0],pipeMon_min[1]);
    }
   
}

int monitor(int pipeLectura, int pipeEscritura){
    int solucion,busq, nbytes, parSol[2];
    /*Es necesario leer de la tubería la solucion y el numero a buscar */
    /*Creacion del fork monitor-terminal*/
    
    /*Impresion de la solucion*/
    /*Cierre de lectura al final de en el hijo */
        do
        {
            nbytes = read(pipeLectura, &parSol, sizeof(int)*2);
            if (nbytes == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
        } while (nbytes != 0);
        solucion = parSol[0];
        busq = parSol[1];
        if(pow_hash(busq)==solucion){
            printf("\nSolution accepted: %08ld --> %08ld", busq, solucion);
        }
        else{
            printf("\nSolution rejected: %08ld !-> %08ld", busq, solucion);
            printf("\nThe soluction has been invalidated");
            printf("\nMonitor exited with status 0");
            printf("\nMiner exited with status 0");
        }
        /*Comunicar al minero que*/
    
}

long minar(int nHilos,long nbusquedas, long busq, int pipeLectura,int pipeEscritura){
    int i,j, rc[MAX_HILOS],parSol[2],nbytes;
    entradaHash t[MAX_HILOS];
    long solucion;
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];

    for ( j = 0 ; j < nbusquedas ; j++)
    {
        for (i = 0; i < nHilos; i++)
        {
            t[i].ep = ((long)MAX / nHilos) * i;
            t[i].eu = ((long)MAX / nHilos) * (i + 1);
            t[i].res = busq;
            rc[i] = pthread_create(&threads[i], NULL, func_minero, (void *)&t[i]);

            if (rc[i])
            {
                printf("Error creando el hilo %d", i);
                return -1;
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
            if((long)sol[i]!=-1){
                solucion = (long)sol[i];
            }
        }
        parSol[0]=solucion;
        parSol[1]=busq;
        do
        {
            nbytes = write(pipeEscritura, &parSol, sizeof(int)*2);
            if (nbytes == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
        } while (nbytes != 0);
        /*Código de comunicación de la solucion al proceso hijo (monitor)*/
        busq=solucion;
        /*Comprobar que es correcto*/
    }
    return 0;     
}