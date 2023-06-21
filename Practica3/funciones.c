/**
 * @file funciones.c
 * @author Diego Rodríguez y Alejandro García
 * @brief
 * @version 3
 * @date 2023-04-1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>

#include "funciones.h"

#define FILE_NAME "mrush_sequence.dat"
#define QUEUE_SIZE 6
#define N_SIZE 4

#define SIZE 7
#define MQ_NAME "/mqueue"

typedef enum
{
    OBJ,
    SOL,
    RES
} NUMS; /*!< Enumeration of the different num values */

struct _Shm_struct
{
    sem_t sem_vacio, sem_mutex, sem_lleno;
    long num[N_SIZE][QUEUE_SIZE];
}; /*!< Structure of the messages between Monitor and Comprobador*/
/*Esto debería alojarse en utils ya que lo comparten minero y comprobador*/

/*Función privada que finaliza el programa Comprobador en caso de error, indicandoselo a Monitor*/
STATUS _finish_comprobador(char *str, int fd_shm, Shm_struct *mapped, int n_sems, STATUS retorno)
{
    if (str)
        perror(str);

    if (fd_shm <= 0)
        close(fd_shm);
    if (mapped)
        if (munmap(mapped, sizeof(Shm_struct)) == ERROR)
            retorno = ERROR;
    if (n_sems >= 1)
        sem_destroy(&mapped->sem_vacio);
    if (n_sems >= 2)
        sem_destroy(&mapped->sem_mutex);
    if (n_sems >= 3)
        sem_destroy(&mapped->sem_lleno);
#ifdef TEST
    nanorandsleep();
#endif
    return retorno;
}

STATUS comprobador(int fd_shm, int lag, sem_t *semCtrl)
{
    FILE *f;
    long new_objetivo, new_solucion, obj, sol;
    int res, index = 0; /*Index apunta a la primera direccción vacía*/
    Shm_struct *mapped;
    struct mq_attr attributes;
    mqd_t mq;
    Message msg;

    printf("[%d] Checking blocks...\n", getpid());
    /*Mapeará el segmento de memoria*/
    if (ftruncate(fd_shm, sizeof(Shm_struct)) == ERROR)
        return _finish_comprobador("ftruncate", fd_shm, NULL, 0, ERROR);

#ifdef DEBUG
    printf("Pasa ftruncate %d\n", getpid());
#endif

    if ((mapped = (Shm_struct *)mmap(NULL, sizeof(Shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        return _finish_comprobador("mmap", fd_shm, NULL, 0, ERROR);
    close(fd_shm); /*Cerramos el descriptor ya que tenemos un puntero a la "posición" de memoria compartida*/

#ifdef DEBUG
    printf("Pasa mmap %d \n", getpid());
#endif
#ifdef TEST
    nanorandsleep();
#endif
    /*Inicialización de los semaforos*/
    if (ERROR == sem_init(&(mapped->sem_mutex), SHARED, 1))
        return _finish_comprobador("Sem_init mutex", 0, mapped, 0, ERROR);

    if (ERROR == sem_init(&(mapped->sem_vacio), SHARED, QUEUE_SIZE))
        return _finish_comprobador("Sem_init vacio", 0, mapped, 1, ERROR);

    if (ERROR == sem_init(&(mapped->sem_lleno), SHARED, 0))
        return _finish_comprobador("Sem_init lleno", 0, mapped, 2, ERROR);
#ifdef TEST
    nanorandsleep();
#endif
#ifdef DEBUG
    printf("Semáforos inicializados %d\n", getpid());
#endif
    up(semCtrl); /*Indica a comprobador que los semáforos ya están inicializados*/
    sem_close(semCtrl);

    attributes.mq_maxmsg = SIZE;
    attributes.mq_msgsize = sizeof(Message);
#ifdef TEST
    nanorandsleep();
#endif
    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)ERROR)
        return _finish_comprobador("Open mq en comprobador", 0, mapped, 3, ERROR);

    while (1)
    {
        if (mq_receive(mq, (char *)&msg, sizeof(Message), 0) == ERROR )
        {
            down(&mapped->sem_vacio);
            down(&mapped->sem_mutex);

            mapped->num[OBJ][index] = -1;
            mapped->num[SOL][index] = sol;
            mapped->num[RES][index] = res;
            index = (index + 1) % QUEUE_SIZE;

            up(&mapped->sem_mutex);
            up(&mapped->sem_lleno);
            mq_close(mq);
            return _finish_comprobador("Receive mq en comprobador", 0, mapped, 3, ERROR);
        }
        obj = msg.obj;
        sol = msg.sol;
#ifdef TEST
        nanorandsleep();
#endif
        res = (obj == pow_hash(sol)); /*Comprobación de la solución*/
#ifdef DEBUG
        printf("Comprueba %ld -> %ld: %ld\n", obj, sol, pow_hash(sol));
#endif
        /*Se la trasladará al monitor a través de la memoria compartida*/
        down(&mapped->sem_vacio);
        down(&mapped->sem_mutex);
#ifdef TEST
        nanorandsleep();
#endif
        mapped->num[OBJ][index] = obj;
        mapped->num[SOL][index] = sol;
        mapped->num[RES][index] = res;
        index = (index + 1) % QUEUE_SIZE;
#ifdef TEST
        nanorandsleep();
#endif
        up(&mapped->sem_mutex);
        up(&mapped->sem_lleno);

        /*Realizará una espera de <lag> milisegundos */
        ournanosleep(lag * MILLON);

        if (obj == -1)
        {
#ifdef DEBUG
            printf("FIN %ld -> %ld fin: %ld\n", obj, sol, pow_hash(sol));
#endif
            break;
        }
    }
#ifdef TEST
    nanorandsleep();
#endif
    /*Liberación de recursos y fin*/
    printf("[%d] Finishing...\n", getpid());

    mq_unlink(MQ_NAME);
    return _finish_comprobador(NULL, 0, mapped, 3, OK);
}

STATUS monitor(int fd_shm, int lag, sem_t *semCtrl)
{
    long obj, sol;
    int res, index = 0; /*Index apunta a la primera direccción llena*/
    Shm_struct *mapped;
    STATUS st;

    printf("[%d] Printing blocks...\n", getpid());

    /*Mapeará el segmento de memoria y cerrará el descriptor ya que tenemos un puntero a la "posición" de mamoria compartida*/
    if ((mapped = (Shm_struct *)mmap(NULL, sizeof(Shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
    {
        close(fd_shm);
        error("Error doing mmap in monitor");
    }
    close(fd_shm);
#ifdef TEST
    nanorandsleep();
#endif
    down(semCtrl); /*Espera a que comprobador inicialice los semáforos*/
    sem_close(semCtrl);

    while (1) /*Extraerá un bloque realizando de consumidor en el esquema productor-consumidor*/
    {
        down(&mapped->sem_lleno);
        down(&mapped->sem_mutex);
#ifdef TEST
        nanorandsleep();
#endif
        obj = mapped->num[OBJ][index];
        sol = mapped->num[SOL][index];
        res = mapped->num[RES][index];
        index = (index + 1) % QUEUE_SIZE;
#ifdef TEST
        nanorandsleep();
#endif
        up(&mapped->sem_mutex);
        up(&mapped->sem_vacio);

        if (obj == -1)
        {
#ifdef DEBUG
            printf("MON FIN %ld -> %ldfin: %ld\n", obj, sol, pow_hash(sol));
#endif
            break;
        }
#ifdef TEST
        nanorandsleep();
#endif

        /*Impresión por pantalla del resultado*/
        if (res)
            printf("Solution accepted: %08ld --> %08ld\n", obj, sol);
        else
            printf("Solution rejected: %08ld !-> %08ld\n", obj, sol);

        ournanosleep(lag * MILLON); /*Espera de <lag> milisegundos */
    }
    printf("[%d] Finishing...\n", getpid());
#ifdef TEST
    nanorandsleep();
#endif
    /*Liberación de recursos y fin. Cerramos los recursos abiertos ya que nosotros somos los últimos en usarlos*/
    sem_destroy(&mapped->sem_vacio);
    sem_destroy(&mapped->sem_mutex);
    sem_destroy(&mapped->sem_lleno);
    munmap(mapped, sizeof(Shm_struct));

    return OK;
}