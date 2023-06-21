/**
 * @file vot_utils.h
 * @author Diego Rodríguez y Alejandro García
 * @brief 
 * @version 
 * @date 2023-03-23
 *
 */
#ifndef VOT_UTILS_H
#define VOT_UTILS_H

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define NOMBREFICHERO "hijosPID.txt"
#define NO_PID -1
#define BIG_PRIME 573163

typedef enum
{
  ERROR = -1,
  OK
} STATUS; /*!< Enumeration of the different status values */

/**
 * @brief Adds 1 unit to the semaphore
 * @author Alejandro García and Diego Rodríguez
 *
 * @return Returns 0 on succes, -1 on Error and more info in errno
 */
int up(sem_t *sem);

/**
 * @brief Blocking call to get the semaphore
 * @author Alejandro García and Diego Rodríguez
 *
 * @return Returns 0 on succes, -1 on Error and more info in errno
 */
int down(sem_t *sem);

/**
 * @brief It is a non blocking down
 * @author Alejandro García and Diego Rodríguez
 *
 * @return Returns 0 on succes, -1 on Error and more info in errno
 */
int down_try(sem_t *sem);

/**
 * @brief Finishes the processes and waits for the sons
 * @author Alejandro García and Diego Rodríguez
 *
 * @return nothing
 */
void end_processes(int n_procs);

/**
 * @brief Closes the semaphores and exits with failure
 * @author Alejandro García and Diego Rodríguez
 *
 * @return nothing
 */
void end_failure(sem_t *semV, sem_t *semC);

/**
 * @brief Changes the handler of the specified signals
 * @author Alejandro García and Diego Rodríguez
 *
 * @return OK, if everything goes well or ERROR if there was some mistake
 */
STATUS set_handlers(int *sig, int n_signals, struct sigaction *actSIG, sigset_t *oldmask, void (*handler)(int));

/**
 * @brief Sends a signal to all the sons except the one specified in the third argument
 * @author Alejandro García and Diego Rodríguez
 *
 * @return OK, if everything goes well or ERROR if there was some mistake
 */
STATUS send_signal_procs(int sig, int n_hijos, long pid);

/**
 * @brief Waits a random number of nanoseconds
 * @author Alejandro García and Diego Rodríguez
 *
 * @return Nothing
 */
void nanorandsleep();

/**
 * @brief Waits a given number of nanoseconds
 * @author Alejandro García and Diego Rodríguez
 *
 * @return Nothing
 */
void ournanosleep(int t);

#endif