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
#include <mqueue.h>

#include "pow.h"
#include "utils.h"

#define MAX_ROUNDS 1000
#define INIC 2
#define SIZE 7 /*Debe ser menor o igual a 10*/
#define MQ_NAME "/mqueue"
#define N_HILOS 16
#define MAX POW_LIMIT - 1

int encontrado;

typedef struct
{
    long ep, eu, res;
} Entrada_Hash;

int minar(int obj);
void *func_minero(void *arg);

/*Función que finaliza el programa en caso de error, indicandoselo a Comprobador*/
void _error_minero(char *str, mqd_t mq, Message msg)
{
    if (str)
        perror(str);
    if (mq)
    { /*Enviar mensaje indicando que se ha acabado*/
        msg.obj = -1;
        (void)mq_send(mq, (char *)(&msg), sizeof(Message), 0);

        mq_close(mq);
    }

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int n_rounds, lag, i; /*Indica la posición a insertar el siguiente mensaje en el array msg*/
    struct mq_attr attributes;
    mqd_t mq;
    Message msg;

    if (argc != 3) /*Control de parámetros de entrada*/
    {
        fprintf(stderr, "Usage: %s <ROUNDS> <LAG>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((n_rounds = atoi(argv[1])) < 1 || n_rounds > MAX_ROUNDS || (lag = atoi(argv[2])) < 1 || lag > MAX_LAG)
    {
        fprintf(stderr, "%d < <N_ROUNDS> < %d and %d < <N_SECS> < %d\n", 1, MAX_ROUNDS, 1, MAX_LAG);
        exit(EXIT_FAILURE);
    }

    /*Creará una cola de mensajes de capacidad SIZE*/
    attributes.mq_maxmsg = SIZE;
    attributes.mq_msgsize = sizeof(Message);

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)ERROR)
        _error_minero(" mq_open en minar", mq, msg);

    printf("[%d] Generating blocks...\n", getpid());

    /*Bucle de minería y mandar las soluciones*/
    for (i = 0, msg.obj = INIC; i < n_rounds; i++, msg.obj = msg.sol)
    {
        /*Construcción del mensaje*/
        if ((msg.sol = minar(msg.obj)) == ERROR)
            _error_minero("Error en minar", mq, msg);
#ifdef DEBUG
        printf("Minamos %d->%d-\n", msg.obj, msg.sol);
#endif
        /*Enviar el mensaje a Comprobador*/
        if (mq_send(mq, (char *)(&msg), sizeof(Message), 0) == ERROR)
            _error_minero(" mq_send ", mq, msg);

        ournanosleep(lag * MILLON); /*Espera de <lag> milisegundos*/
    }

    /*Enviar mensaje indicando que se ha acabado*/
    msg.obj = -1;
    if (mq_send(mq, (char *)(&msg), sizeof(Message), 0) == ERROR)
        _error_minero(" mq_send ", mq, msg);

    /*Liberación de recursos y finalización*/
    printf("[%d] Finishing...\n", getpid());
    mq_close(mq);
    exit(EXIT_SUCCESS);
}

int minar(int obj)
{

    int i, incr = ((int)MAX) / N_HILOS;
    long solucion;
    Entrada_Hash t[N_HILOS];
    pthread_t threads[N_HILOS];
    void *sol[N_HILOS];

    encontrado = 0;
    /*Creación de hilos*/
    for (i = 0; i < N_HILOS; i++)
    {
        t[i].ep = incr * i;
        if (i == N_HILOS - 1)
            t[i].eu = MAX;
        else
            t[i].eu = incr * (i + 1);
        t[i].res = obj;

        if (pthread_create(&threads[i], NULL, func_minero, (void *)(t + i)))
        {
            for (; i >= 0; i--)
                (void)pthread_join(threads[i], sol + i);

            return ERROR;
        }
    }

    /*Joins de los hilos*/
    for (i = 0; i < N_HILOS; i++)
    {
        if (pthread_join(threads[i], sol + i))
        {
            for (i++; i < N_HILOS; i++)
                (void)pthread_join(threads[i], sol + i);
            perror(" mq_open en la unión de hilos");
            return ERROR;
        }
        if ((long)sol[i] != -1)
            solucion = (long)sol[i];
    }

    return (int)solucion;
}

void *func_minero(void *arg)
{
    long i;
    Entrada_Hash *e = (Entrada_Hash *)arg;

    for (i = e->ep; (i < e->eu) && (encontrado == 0); i++)
    {
        if (pow_hash(i) == e->res)
        {
            encontrado = 1;
            pthread_exit((void *)i);
        }
    }

    pthread_exit((void *)ERROR);
}