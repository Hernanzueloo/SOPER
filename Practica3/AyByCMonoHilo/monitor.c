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
#define MAX_LAG 1000000    /*Un segundo*/


int main(int argc, char* argv[]){
    sem_t *semCtrl;
    int lag, fd;
    /*Control error*/
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <LAG>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /*Retraso en milisegundos entre cada monitorización*/
    lag = atoi(argv[1]);

    if(lag < 1 || lag > MAX_LAG){
        fprintf(stderr, "%d < <LAG> < %d\n",1,MAX_LAG);
        exit(EXIT_FAILURE);
    }
    if ((semCtrl = sem_open(NAME_SEM_CTRL, O_CREAT , S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
    {
        perror("sem_open SemCtrl");
        exit(EXIT_FAILURE);
    }

   /*Aqui se debe hacer una petición a memoria compartida para que los procesos divergan*/
    fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR |S_IWUSR);
    if(fd == ERROR){

        if(errno ==  EEXIST){

            fd = shm_open(SHM_NAME, O_RDWR, 0); 
            if(fd == ERROR){
                perror (" Error opening the shared memory segment ") ;
                /*Controlar al otro proceso*/
                exit(EXIT_FAILURE);
            }else{
                /*EL programa diverge un se convierte en monitor y otro en comprobador*/
                #ifdef DEBUG
                    printf("Monitor %d\n", getpid());
                #endif
                if( monitor(fd,lag,semCtrl)==ERROR){
                    perror ("Error in monitor") ;
                    exit(EXIT_FAILURE);
                }
                /*Se desvinculan ambos recursos al ser nosotros los últimos en usarlos*/
                shm_unlink (SHM_NAME);
                sem_unlink(NAME_SEM_CTRL);
            }
        }else{
            perror (" Error creating the shared memory segment ") ;
            /*Controlar al otro proceso*/
            exit(EXIT_FAILURE);
        }
    }else{
        /*EL programa diverge un se convierte en monitor y otro en comprobador*/
        #ifdef DEBUG
            printf("Comprobador %d\n", getpid());
        #endif
        if(comprobador(fd,lag,semCtrl)==ERROR){
            perror ("Error in comprobador") ;
            shm_unlink(SHM_NAME);
            exit(EXIT_FAILURE);
        }
        
        
    }

     exit(EXIT_SUCCESS);
}


/*Idea general del progrma:*/

/*Se busca que al ejecutar desde terminales diferentes, minero y monitro (2 veces) con retardos diferentes, el progrma funcione correctamente*/
/*Mirar imagenes última hoja de la practica*/

