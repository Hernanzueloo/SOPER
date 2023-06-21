#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pow.h"
#include "miner.h"
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int newpid, Status;

    /*Control de errores*/
    if ((atoi(argv[1]) < 0) || (atoi(argv[1]) > POW_LIMIT) || (atoi(argv[2]) < 0) || (atoi(argv[3]) < 0) || (atoi(argv[3]) > MAX_HILOS))
    {
        printf("\n\nError en los parametros de entrada\n");
        return 1;
    }

    /*Buscamos los elementos*/
    newpid = fork();
    if (newpid)

    {
        wait(&Status);
        if (WIFEXITED(Status)) {
            printf("Miner exited with status %d\n", WEXITSTATUS(Status));
        } else if (WIFSIGNALED(Status)) {
            printf("Miner terminated by OS\n");
        }
        
    }
    else
    {
        minero(atoi(argv[3]), atoi(argv[2]), atol(argv[1])); /*Queremos que minero devuelve el proximo numero a buscar*/
    }

    return 0;
}