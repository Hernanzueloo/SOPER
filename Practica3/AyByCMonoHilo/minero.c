
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"
#include <mqueue.h>

#include "pow.h"
#include "utils.h"

#define MAX_ROUNDS 1000
#define MAX_LAG 1000000    /*Un segundo*/
#define INIC 0
#define SIZE 7 /*Debe ser menor o igual a 10*/
#define MQ_NAME "/mqueue"

int minar(int obj);

int main(int argc, char *argv[])
{
    int n_rounds, lag, i, index; /*Indica la posición a insertar el siguiente mensaje en el array msg*/
    struct mq_attr attributes;
    Message msg[SIZE+1]; /*Número de mensajes que deben estar listos como mucho los 7 en el mailbox y el siguiente a enviar*/
    mqd_t mq;
    /*Control error*/
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <ROUNDS> <LAG>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /*El numero de rondas que se va a realizar */
    n_rounds = atoi(argv[1]);
    /*el retraso en milisegundos entre cada ronda*/
    lag = atoi(argv[2]);
    
    if (n_rounds < 1 || n_rounds > MAX_ROUNDS|| lag < 1 || lag > MAX_LAG){
        fprintf(stderr, "%d < <N_PROCS> < %d and %d < <N_SECS> < %d\n",1,MAX_ROUNDS,1,MAX_LAG);
        exit(EXIT_FAILURE);
    }

    /*Creará una cola de mensajes de capacidad 7*/
    attributes.mq_maxmsg = SIZE;
    attributes.mq_msgsize = sizeof(Message) ;

    if (( mq = mq_open ( MQ_NAME , O_CREAT | O_RDWR , S_IRUSR | S_IWUSR , &attributes ) ) == ( mqd_t )ERROR) {
         perror (" mq_open ") ;
         exit ( EXIT_FAILURE ) ;
    }
    printf("[%d] Generating blocks...\n",getpid());

    /*Establecerá un objetivo inical fijo para la POW <INIC> (macro)*/
    msg[0].obj=INIC;

    /*Para cada ronda resolverá la POW */
    for(i=0,index=0;i<n_rounds;i++){
        
        /*Una vez terminada las rondas enviará un bloque especial indicandole la finalización*/
        msg[index].fin = (int)(i+1)/n_rounds;

        /*No se si con la función ya creada o con otra nueva (Parece que con una versión de la de la practica 1)*/
        msg[index].sol=minar(msg[index].obj);

         /*Enviará por la cola de mensajes (al Comprobador ) un mensaje con al menos el objetivo y la solución hallada*/
        if(mq_send ( mq , (char*)(msg+index), sizeof(msg[index]), 0) == ERROR){
            perror (" mq_send ") ;
            mq_close ( mq ) ;
            exit ( EXIT_FAILURE ) ;
        }

        /*Realizará una espera de <LAG> milisegundos*/
        ournanosleep(lag*MIL);
        /*Establezerá como siguiente objetivo la solución anterior*/
        index = (i+1)%(SIZE+1);
        msg[index].obj =msg[i%(SIZE+1)].sol;
    }
    printf("[%d] Finishing...\n",getpid());

     /*Liberará recursos y terminará*/
    mq_close(mq);
    
    exit(EXIT_SUCCESS);
}

int minar(int obj){
    int i;
    int x =-1;

    for (i = 0; (i <POW_LIMIT) && (x <= 0); i++)
    {
        if ((long)pow_hash(i) == obj)
        {
            x = i;
        }
    }
    return x;
}