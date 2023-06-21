
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"

#include "pow.h"

#define MAX_ROUNDS 1000
#define MAX_LAG 1000000    /*Un segundo*/
#define INIC 0

int main(int argc, char *argv[])
{

    int n_rounds, lag;

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

    /*Establecerá un objetivo inical fijo para la POW <INIC> (macro)*/

    /*Para cada ronda resolverá la POW */
    /*No se si con la función ya creada o con otra nueva (Parece que con una versión de la de la practica 1)*/

    /*Enviará por la cola de mensajes (al Comprobador ) un mensaje con al menos el objetivo y la solución hallada*/

    /*Realizará una espera de <LAG> milisegundos*/

    /*Establezerá como siguiente objetivo la solución anterior*/

    /*Una vez terminada las rondas enviará un bloque especial indicandole la finalización */

    /*Liberará recursos y terminará*/

}