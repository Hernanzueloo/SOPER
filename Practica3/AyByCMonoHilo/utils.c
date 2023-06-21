/**
 * @file utils.h
 * @author Diego Rodríguez y Alejandro García
 * @brief 
 * @version 
 * @date 2023-04-1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#include "utils.h"

/*Adds 1 unit to the semaphore*/
int up(sem_t *sem)
{
    return sem_post(sem);
}

/*Blocking call to get the semaphore*/
int down(sem_t *sem)
{
    return sem_wait(sem);
}

/*It is a non blocking down*/
int down_try(sem_t *sem)
{
    return sem_trywait(sem);
}

/*Waits a random number of nanoseconds*/
void nanorandsleep(){
  struct timespec time ={0,rand()%BIG_PRIME};
  nanosleep(&time, NULL);
}

/*Waits a given number of nanoseconds*/
void ournanosleep(int t){
struct timespec time ={0,t};
  nanosleep(&time, NULL);
}