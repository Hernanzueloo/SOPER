/**
 * @file monitor.c
 * @author Diego Rodríguez y Alejandro García
 * @brief
 * @version 3
 * @date 2023-04-1
 *
 */
/*Quitar Mutex*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "funciones.h"

#define SHM_NAME "/shm_name"
#define NAME_SEM_CTRL "/semCTRL"

int main(int argc, char *argv[])
{
    sem_t *semCtrl;
    int lag, fd;
    STATUS st;

    if (argc != 2) /*Control de errores*/
    {
        fprintf(stderr, "Usage: %s <LAG>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lag = atoi(argv[1]); /*Retraso en milisegundos entre cada monitorización*/

    if (lag < 1 || lag > MAX_LAG)
    { /*Control de errores*/
        fprintf(stderr, "%d < <LAG> < %d\n", 1, MAX_LAG);
        exit(EXIT_FAILURE);
    }

    if ((semCtrl = sem_open(NAME_SEM_CTRL, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) /*Apertura de semáforo*/
        error("sem_open SemCtrl");

    /*Petición a memoria compartida para que los procesos divergan*/
    if ((fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == ERROR)
    {
        if (errno == EEXIST)
        {
            if ((fd = shm_open(SHM_NAME, O_RDWR, 0)) == ERROR) /*Control de errores*/
                error(" Error opening the shared memory segment ");
            else /*Caso monitor*/
            {
#ifdef DEBUG
                printf("Monitor %d\n", getpid());
#endif
                if (monitor(fd, lag, semCtrl) == ERROR)
                    perror("Error in monitor");
                /*Se desvinculan ambos recursos al ser nosotros los últimos en usarlos*/
                shm_unlink(SHM_NAME);
                sem_unlink(NAME_SEM_CTRL);
            }
        }
        else /*Control de errores*/
            error(" Error creating the shared memory segment ");
    }
    else /*Caso comprobador*/
    {
#ifdef DEBUG
        printf("Comprobador %d\n", getpid());
#endif
        if (comprobador(fd, lag, semCtrl) == ERROR)
            error("Error in comprobador");
    }

    exit(EXIT_SUCCESS);
}