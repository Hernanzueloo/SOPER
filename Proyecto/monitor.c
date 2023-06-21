/**
 * @file minero.c
 * @author Diego Rodríguez y Alejandro García
 * @brief
 * @version 3
 * @date 2023-04-1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "funciones_monitor.h"

#define SHM_NAME2 "/shm_name"
#define NAME_SEM_CTRL "/semCTRL"
#define SEM_NAME_MIN "/semMin"

int main()
{
    sem_t *semCtrl;
    sem_t *semMin;
    int fd;

    if ((semCtrl = sem_open(NAME_SEM_CTRL, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) /*Apertura de semáforo*/
        error("sem_open SemCtrl");

    if((semMin =  sem_open(SEM_NAME_MIN, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
        error("sem_open semMMin");
    }

    switch (fork())
    {
    case -1: /*In case of error*/
        error("Error forking");
        break;

    case 0: /*Monitor*/
        down(semCtrl);
        if ((fd = shm_open(SHM_NAME2, O_RDWR, 0)) == ERROR) /*Control de errores*/
            error(" Error opening the shared memory segment ");
        if (monitor(fd) == ERROR)
            error("Error in monitor");

        exit(EXIT_SUCCESS);
        break;

    default: /*Comprobador*/
        if ((fd = shm_open(SHM_NAME2, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == ERROR)
            error(" Error opening the shared memory segment ");
        up(semMin);
        if (comprobador(fd, semCtrl) == ERROR)
            error("Error in comprobador");
        break;
    }

    down(semMin);
    sem_close(semMin);
    /*Se desvinculan todos los recursos al ser nosotros los últimos en usarlos*/
    shm_unlink(SHM_NAME2);
    sem_unlink(NAME_SEM_CTRL);

    exit(EXIT_SUCCESS);
}