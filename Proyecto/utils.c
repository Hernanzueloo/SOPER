/**
 * @file utils.c
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
#include <mqueue.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


#include "utils.h"

int up(sem_t *sem)
{
  return sem_post(sem);
}

int down(sem_t *sem)
{
  return sem_wait(sem);
}

int down_try(sem_t *sem)
{
  return sem_trywait(sem);
}


/*Changes the handler of the specified signals*/
STATUS set_handlers(int *sig, int n_signals, struct sigaction *actSIG, sigset_t *Signals, sigset_t *No_signals, void (*handler)(int))
{
    int i = 0;

    sigemptyset(Signals);
    sigfillset(No_signals);
    for (i = 0; i < n_signals; i++)
    {
        if (sigaddset(Signals, sig[i]) == ERROR)
            return ERROR;
        if (sigdelset(No_signals, sig[i]) == ERROR)
            return ERROR;
    }

    /*Block signals*/
    if (sigprocmask(SIG_BLOCK, Signals, NULL) == ERROR)
        return ERROR;

    if(handler)
        actSIG->sa_handler = handler;
    else
        actSIG->sa_handler = SIG_IGN;


    /*Set the new signal handler*/
    sigemptyset(&(actSIG->sa_mask));
    actSIG->sa_flags = 0;

    for (i = 0; i < n_signals; i++)
        if (sigaction(sig[i], actSIG, NULL) == ERROR)
            return ERROR;

    /*Unblock signals*/
    if (sigprocmask(SIG_UNBLOCK, Signals, NULL) == ERROR)
        return ERROR;
    return OK;
}

void print_bloque(int fd, Bloque *bloque)
{
  int i;

  if (fd < 0 || !bloque)
    return;

  /*Formato del bloque: */
  /*
  Id :  <Id del bloque>
  Winner : <PID>
  Target: <TARGET>
  Solution: <Solución propuesta>
  Votes : <N_VOTES_ACCEPT>/<N_VOTES>
  Wallets : <PID>:<N_MONEDAS> ...
  */

  dprintf(fd, "\nId:  %ld \n", bloque->id);
  dprintf(fd, "Winner:  %d \n", bloque->pid);
  dprintf(fd, "Target:  %ld \n", bloque->obj);

  dprintf(fd, "Solution:  %ld \n", bloque->sol);

  dprintf(fd, "Votes:  %ld/%ld\n", bloque->votos_a, bloque->n_votos);
  dprintf(fd, "Wallets:");

  for (i = 0; i < MAX_MINERS; i++)
    if (wallet_get_pid(&(bloque->Wallets[i])) != 0)
      dprintf(fd, " %d:%d ", wallet_get_pid(&(bloque->Wallets[i])), wallet_get_coins(&(bloque->Wallets[i])));
  dprintf(fd, " \n");
}

void nanorandsleep()
{
  struct timespec time = {0, rand() % BIG_PRIME};
  nanosleep(&time, NULL);
}

void ournanosleep(long t)
{
  long aux = t;
  struct timespec time;

  t = t / MIL;
  t = t / MIL;
  t = t / MIL;
  time.tv_sec = (time_t)t;
  time.tv_nsec = (time_t)aux - t * MIL * MIL * MIL;
  nanosleep(&time, NULL);
}

void error(char *str)
{
  if (str)
    perror(str);

  exit(EXIT_FAILURE);
}

void copy_block(Bloque *dest, Bloque *orig)
{
  int i;

  if (orig == NULL || dest == NULL)
    return;

  dest->id = orig->id;
  dest->obj = orig->obj;
  dest->sol = orig->sol;

  dest->votos_a = orig->votos_a;
  dest->n_votos = orig->n_votos;
  dest->n_mineros = orig->n_mineros;

  dest->pid = orig->pid;

  for (i = 0; i < MAX_MINERS; i++)
    copy_wallet(&(dest->Wallets[i]), &(orig->Wallets[i]));
}

void copy_wallet(Wallet *dest, Wallet *orig)
{
  if (orig == NULL || dest == NULL)
    return;

  dest->pid = orig->pid;
  dest->coins = orig->coins;
}

int wallet_get_coins(Wallet *wallet)
{
  if (!wallet)
    return -1;

  return wallet->coins;
}

void wallet_set_coins(Wallet *wallet, int coins)
{
  if (!wallet)
    return;

  wallet->coins = coins;
}

pid_t wallet_get_pid(Wallet *wallet)
{
  if (!wallet)
    return -1;

  return wallet->pid;
}

void wallet_set_pid(Wallet *wallet, pid_t pid)
{
  if (!wallet)
    return;

  wallet->pid = pid;
}

STATUS block_all_signal(sigset_t *oldmask)
{
  sigset_t mask2;

  sigfillset(&mask2);
  /*Block signals*/
  return sigprocmask(SIG_BLOCK, &mask2, oldmask);
}

void send_signals_miners(Wallet * w, int no_index, int signal){
  int i;
  
  if(!w){
    return;
  }
    for (i = 0; i < MAX_MINERS; i++)
    {
        if (wallet_get_pid(&(w[i])) != 0 && no_index != i )
        {
            kill(wallet_get_pid(&(w[i])), signal);
                /*perror("Error sending signal");*/
        }
    }
}
