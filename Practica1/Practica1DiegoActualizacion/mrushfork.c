#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pow.h"

#define MAX POW_LIMIT-1
#define MAX_HILOS 100

int encontrado;

typedef struct
{
    long ep, eu, res;
} entradaHash;

void *func_minero(void *arg);

int monitor(int pipeLectura, int pipeEscritura,int nbusquedas);

long minero(int nHilos,long nbusquedas, long busq);


long minar(int nHilos,long nbusquedas, long busq, int pipeLectura,int pipeEscritura);

int main(int argc, char *argv[])
{
    int rc[MAX_HILOS], t1, t2, i,newpid,Status;
    long solucion, busq;
    struct timespec inicio, fin;
    double tiempo_transcurrido;

    // Obtenemos el tiempo actual
    clock_gettime(CLOCK_REALTIME, &inicio);
    /*Control de errores*/
    
    if ((atoi(argv[1]) < 0) || (atoi(argv[1]) > POW_LIMIT) || (atoi(argv[2]) < 0) || (atoi(argv[3]) < 0) || (atoi(argv[3]) > MAX_HILOS))
    {
        printf("\n\nError en los parametros de entrada");
        return 1;
    }
    
    /*Buscamos los elementos*/
    busq = atol(argv[1]);
    t1 = clock();
    newpid=fork();
    if(newpid){
            wait(&Status); 
            printf("Miner exited with status %d\n",Status);

        
    }else{
       
        busq = minero(atoi(argv[3]),atoi(argv[2]), busq); /*Queremos que minero devuelve el proximo numero a buscar*/

    }
    t2=clock();
    clock_gettime(CLOCK_REALTIME, &fin);
    tiempo_transcurrido = (double)(fin.tv_sec - inicio.tv_sec) + (double)(fin.tv_nsec - inicio.tv_nsec) / 1000000000.0;
        printf("El tiempo de ejecución fue de %.9f segundos.\n", tiempo_transcurrido);

    return 0;
}

void *func_minero(void *arg)
{
    int i;
    long x = -1;
    entradaHash *e = (entradaHash *)arg;

    for (i = e->ep; (i < e->eu) && (x <= 0)&&(encontrado==0); i++)
    {
        if ((long)pow_hash(i) == e->res)
        {
            x = i;
            encontrado=1;
        }
    }

    pthread_exit((void *)x);
}

long minero(int nHilos, long nbusquedas, long busq)
{
    int newpid,status, pipeMon_min[2], pipeMin_mon[2];
    /*Creación de las pipeline MON->MIN y MIN->MON*/
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
    /*Creacion del proceso monitor*/
    newpid=fork();
    if(newpid){
        /*Proceso MIN*/
        /*Cierro los ficheros que no vamos a usar*/
        close(pipeMon_min[1]); /*Escritura MON->MIN*/
        close(pipeMin_mon[0]); /*Lectura MIN->MON*/
         /*Funcion MINERO que crea todos los hilos*/
        if(minar(nHilos,nbusquedas,busq,pipeMon_min[0],pipeMin_mon[1])==0){
            return -1;
        }
        
    }else{
        /*Código del hijo, monitor*/
         /*Cierro los ficheros que no vamos a usar*/
        close(pipeMin_mon[1]);/*Escritura MIN->MON*/
        close(pipeMon_min[0]); /*Lectura MON->MIN*/
         /*Funcion MONITOR */
        monitor(pipeMin_mon[0],pipeMon_min[1],nbusquedas);
    }
   
}

int monitor(int pipeLectura, int pipeEscritura,int nBusquedas){
    int  nbytes, parSol[2], Exito=0;
    long solucion,busq;
    /*Es necesario leer de la tubería la solucion y el numero a buscar */
    /*Creacion del fork monitor-terminal*/
    
    /*Impresion de la solucion*/
    /*Cierre de lectura al final de en el hijo */
    while (1)  {
        nbytes = read(pipeLectura, &parSol, sizeof(int)*2);
        if (nbytes == -1)
        {
            printf("\nERROR 146\n");
            perror("read");
            close(pipeEscritura);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        if (nbytes==0){
            close(pipeEscritura);
            close(pipeLectura);
            if( nBusquedas==0)
                exit(EXIT_SUCCESS);
            printf("\nERROR 152\n");
            exit(EXIT_FAILURE);
        }
        solucion = (long)parSol[0];
        busq = (long)parSol[1];
        nBusquedas--;
        if(pow_hash(solucion)==busq){
            printf("Solution accepted: %08ld --> %08ld\n", busq, solucion);
            Exito=1;
        }else{
            printf("Solution rejected: %08ld !-> %08ld\n", busq, solucion);
            printf("The soluction has been invalidated\n");
            Exito=0;
        }

        /*Comunicar al minero que ha acertado o no*/
       
        nbytes = write(pipeEscritura, &Exito, sizeof(int)*1);
        if (nbytes == -1)
        {
            printf("\nERROR\n");
            perror("read");
            close(pipeEscritura);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
       
       
      }   
}

long minar(int nHilos,long nbusquedas, long busq, int pipeLectura,int pipeEscritura){
    int i,j,incr, rc[MAX_HILOS],parSol[2],nbytes,Exito=0,Status;
    entradaHash t[MAX_HILOS];
    int solucion;
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];

    incr=((int)MAX) / nHilos;
    for ( j = 0 ; j < nbusquedas ; j++)
    {   
        encontrado=0;
        for (i = 0; i < nHilos; i++)
        {
            t[i].ep = incr * i;
            t[i].eu = incr* (i + 1);
            if( i == nHilos-1){ /*Para el caso en el que MAX no sea divisible por NHilos*/
                t[i].eu = MAX;
             }
            t[i].res = busq;
            rc[i] = pthread_create(&threads[i], NULL, func_minero,  (void *)(t+i));

            if (rc[i])
            {
                printf("\nERROR 210\n");
                perror("Thread creation");
                close(pipeEscritura);
                wait(&Status);
                printf("Monitor exited with status %d\n",Status);
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
        }
        /*Joins de los hilos*/
        for (i = 0; i < nHilos; i++)
        {
            rc[i] = pthread_join(threads[i],  sol+i);
            if (rc[i])
            {
                printf("\nERROR 223\n");
                perror("Thread joining");
                close(pipeEscritura);
                wait(&Status);
                 printf("Monitor exited with status %d\n",Status);
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
        }
        for (i = 0; i < nHilos; i++)
        {
            if((int)sol[i]!=-1){
                solucion = (int)sol[i];
                break;

            }
        }
        parSol[0]=solucion;
        parSol[1]=busq;
        
        nbytes = write(pipeEscritura, &parSol, sizeof(int)*2);
        if (nbytes == -1)
        {
            printf("\nERROR 243\n");
            perror("read");
            close(pipeEscritura);
            wait(&Status);
            printf("Monitor exited with status %d\n",Status);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        
        
        /*Código de comunicación de la solucion al proceso hijo (monitor)*/
       
        nbytes = read(pipeLectura, &Exito, sizeof(int)*1);
        if (nbytes == -1)
        {
            perror("read");
            close(pipeEscritura);
            wait(&Status);
            printf("Monitor exited with status %d\n",Status);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        if (nbytes==0){
            printf("\nERROR 263\n");
            perror("read");
            close(pipeEscritura);
            wait(&Status);
            printf("Monitor exited with status %d\n",Status);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        
        if(!Exito){
            printf("\nERROR 271\n");

            close(pipeEscritura);
            wait(&Status);
            printf("Monitor exited with status %d\n",Status);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        busq=solucion;
        /*Comprobar que es correcto*/

    }
    close(pipeEscritura);
    wait(&Status);
    printf("Monitor exited with status %d\n",Status);
    close(pipeLectura);
    exit(EXIT_SUCCESS);
    return 0;    
}