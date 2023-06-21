#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "miner.h"

int encontrado;

struct _entradaHash
{
    long ep, eu, res;
};

/*Declaracion de funciones privadas*/
/**
 * @brief Crea un hilo que busca la solucion entre dos parametros
 *
 * @param arg estructura que almacena el rango en el que buscar y la solución a buscar
 * 
 * @return un puntero con el resultado de la busqueda
 */
void *func_minero(void *arg);

/**
 * @brief Imprime y comprueba las soluciones
 *
 * @param pipeLectura pipe del que se leen las soluciones
 * @param pipeEscritura pipe en el que se escriben las soluciones
 * @param nbusquedas numero de rondas que se haran
 * 
 * @return EXIT_SUCCESS, si todo va bien or EXIT_FAILURE en caso de error
 */
void monitor(int pipeLectura, int pipeEscritura,int nbusquedas);

/**
 * @brief Gestiona los hilos
 *
 * @param nHilos numero de hilos a crear
 * @param nbusquedas numero de rondas que se haran
 * @param busq solucion a buscar
 * @param pipeLectura pipe del que se leen las soluciones
 * @param pipeEscritura pipe en el que se escriben las soluciones
 * 
 * @return nada
 */
void minar(int nHilos, long nbusquedas, long busq, int pipeLectura,int pipeEscritura);


void *func_minero(void *arg)
{
    int i;
    long x = -1;
    entradaHash *e = (entradaHash *)arg;

    for (i = e->ep; (i < e->eu) && (x <= 0) && (encontrado == 0); i++)
    {
        if ((long)pow_hash(i) == e->res)
        {
            x = i;
            encontrado = 1;
        }
    }

    pthread_exit((void *)x);
}

void monitor(int pipeLectura, int pipeEscritura, int nBusquedas)
{
    int nbytes, Exito = 0;
    long solucion, busq, parSol[2]; /*Es necesario leer de la tubería la solucion y el numero a buscar */

    while (1)
    {
        nbytes = read(pipeLectura, &parSol, sizeof(long) * 2);
        if (nbytes == -1)
        {
            perror("read monitor");
            close(pipeEscritura);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
        if (nbytes == 0)
        {
            close(pipeEscritura);
            close(pipeLectura);
            if (nBusquedas == 0 && pow_hash(solucion) == busq)
                exit(EXIT_SUCCESS);
            exit(EXIT_FAILURE);
        }
        solucion = (long)parSol[0];
        busq = (long)parSol[1];
        nBusquedas--;
        if (pow_hash(solucion) == busq)
        {
            printf("Solution accepted: %08ld --> %08ld\n", busq, solucion);
            Exito = 1;
        }
        else
        {
            printf("Solution rejected: %08ld !-> %08ld\n", busq, solucion);
            printf("The soluction has been invalidated\n");
            Exito = 0;
        }

        /*Comunicar al minero que ha acertado o no*/

        nbytes = write(pipeEscritura, &Exito, sizeof(int) * 1);
        if (nbytes == -1)
        {
            perror("write monitor");
            close(pipeEscritura);
            close(pipeLectura);
            exit(EXIT_FAILURE);
        }
    }
}

void minar(int nHilos, long nbusquedas, long busq, int pipeLectura, int pipeEscritura)
{
    int i, j, incr, rc[MAX_HILOS], nbytes, Exito = 0, Status;
    entradaHash t[MAX_HILOS];
    long solucion, parSol[2];
    pthread_t threads[MAX_HILOS];
    void *sol[MAX_HILOS];

    incr = ((int)MAX) / nHilos;
    for (j = 0; j <= nbusquedas; j++)
    {
        /*Escribimos el reslutado de la búsqueda anterior*/
        if ( j > 0){
            nbytes = write(pipeEscritura, &parSol, sizeof(long) * 2);
            if (nbytes == -1)
            {
                perror("write miner");
                close(pipeEscritura);
                wait(&Status);
                if (WIFEXITED(Status)) {
                    printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                } else if (WIFSIGNALED(Status)) {
                    printf("Monitor terminated by OS\n");
                }                
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
        }
        if ( j < nbusquedas){
            encontrado = 0;
            /*Creación de hilos*/
            for (i = 0; i < nHilos; i++)
            {
                t[i].ep = incr * i;
                t[i].eu = incr * (i + 1);
                if (i == nHilos - 1)
                { /*Para el caso en el que MAX no sea divisible por NHilos*/
                    t[i].eu = MAX;
                }
                t[i].res = busq;
                rc[i] = pthread_create(&threads[i], NULL, func_minero, (void *)(t + i));

                if (rc[i])
                {
                    perror("Thread creation");
                    close(pipeEscritura);
                    wait(&Status);
                    if (WIFEXITED(Status)) {
                        printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                    } else if (WIFSIGNALED(Status)) {
                        printf("Monitor terminated by OS\n");
                    }
                    
                    close(pipeLectura);
                    exit(EXIT_FAILURE);
                }
            }

            /*Joins de los hilos*/
            for (i = 0; i < nHilos; i++)
            {
                rc[i] = pthread_join(threads[i], sol + i);
                if (rc[i])
                {
                    perror("Thread joining");
                    close(pipeEscritura);
                    wait(&Status);
                    if (WIFEXITED(Status)) {
                        printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                    } else if (WIFSIGNALED(Status)) {
                        printf("Monitor terminated by OS\n");
                    }
                    close(pipeLectura);
                    exit(EXIT_FAILURE);
                }
            }

            for (i = 0; i < nHilos; i++)
            {
                if ((long)sol[i] != -1)
                {
                    solucion = (long)sol[i];
                    break;
                }
            }
            
            parSol[0] = solucion;
            parSol[1] = busq;
        }
        

        /*Código de comunicación de la solucion al proceso hijo (monitor)*/
        if(j > 0 ){
            nbytes = read(pipeLectura, &Exito, sizeof(int) * 1);
            if (nbytes == -1)
            {
                perror("read miner");
                close(pipeEscritura);
                wait(&Status);
                if (WIFEXITED(Status)) {
                    printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                } else if (WIFSIGNALED(Status)) {
                    printf("Monitor terminated by OS\n");
                }
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
            if (nbytes == 0)
            {
                perror("read miner");
                close(pipeEscritura);
                wait(&Status);
                if (WIFEXITED(Status)) {
                    printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                } else if (WIFSIGNALED(Status)) {
                    printf("Monitor terminated by OS\n");
                }                
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
            /*Comprobar que es correcto*/

            if (!Exito)
            {
                close(pipeEscritura);
                if (WIFEXITED(Status)) {
                    printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
                } else if (WIFSIGNALED(Status)) {
                    printf("Monitor terminated by OS\n");
                }                
                close(pipeLectura);
                exit(EXIT_FAILURE);
            }
        }
        
        busq = solucion;
    }

    close(pipeEscritura);
    wait(&Status);
    if (WIFEXITED(Status)) {
        printf("Monitor exited with status %d\n", WEXITSTATUS(Status));
     } else if (WIFSIGNALED(Status)) {
        printf("Monitor terminated by OS\n");
    }    
    close(pipeLectura);
    exit(EXIT_SUCCESS);
}

void minero(int nHilos, long nbusquedas, long busq)
{
    int newpid, status, pipeMon_min[2], pipeMin_mon[2];

    /*Creación de las pipeline MON->MIN y MIN->MON*/
    status = pipe(pipeMon_min);
    if (status == -1)
    {
        perror("pipe creation");
        exit(EXIT_FAILURE);
    }

    status = pipe(pipeMin_mon);
    if (status == -1)
    {
        perror("pipe creation");
        exit(EXIT_FAILURE);
    }

    /*Creacion del proceso monitor*/
    newpid = fork();
    if (newpid)
    {
        /*Proceso MIN*/
        /*Cierro los ficheros que no vamos a usar*/
        close(pipeMon_min[1]); /*Escritura MON->MIN*/
        close(pipeMin_mon[0]); /*Lectura MIN->MON*/
                               /*Funcion MINERO que crea todos los hilos*/
        minar(nHilos, nbusquedas, busq, pipeMon_min[0], pipeMin_mon[1]);
    }
    else
    {
        /*Código del hijo, monitor*/
        /*Cierro los ficheros que no vamos a usar*/
        close(pipeMin_mon[1]); /*Escritura MIN->MON*/
        close(pipeMon_min[0]); /*Lectura MON->MIN*/
                               /*Funcion MONITOR */
        monitor(pipeMin_mon[0], pipeMon_min[1], nbusquedas);
    }
}
