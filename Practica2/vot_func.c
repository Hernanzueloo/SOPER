#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "vot_func.h"

#define NOMBREVOTAR "votos.txt"
#define NSIGNALS 4
#define WAITVOTE 1000000
#define WAITCANDIDATE 250000000

volatile sig_atomic_t got_sigUSR1 = 0;
volatile sig_atomic_t got_sigUSR2 = 0;
volatile sig_atomic_t got_sigTERM = 0;

/*Private function that defines the new handler for the signals SIGTERM, SIGUSR1 and SIGUSR2*/
void _handler_voter(int sig)
{
  switch (sig)
  {
  case SIGTERM:
#ifdef DEBUG
    printf("SIGTERM %ld \n", (long)getpid());
#endif
    got_sigTERM = 1;
    break;
  case SIGUSR1:
#ifdef DEBUG
    printf("USR1 %ld\n", (long)getpid());
#endif
    got_sigUSR1 = 1;
    break;
  case SIGUSR2:
#ifdef DEBUG
    printf("USR2 %ld\n", (long)getpid());
#endif
    got_sigUSR2 = 1;
    break;
  case SIGINT:
#ifdef DEBUG
    printf("SIGINT %ld\n", (long)getpid());
#endif
    break;
  default:
#ifdef DEBUG
    printf("SIGNAL UNKNOWN(%d) %ld\n", sig, (long)getpid());
#endif
    break;
  }
}

/*Sends a signal to end */
void _error_in_voters()
{
#ifdef DEBUG
  printf("ERROR ocurred in the son with PID=%ld\n", (long)getpid());
#endif
  if (kill(getppid(), SIGUSR1))
  {
#ifdef DEBUG
    fprintf(stderr, "ERROR sending the error signal in %ld\n", (long)getppid());
#endif
  }
}

/*Voting when the voter has executed an effective down*/
STATUS votingCarefully(char *nFichV)
{
  FILE *f = NULL;
  char letra;

  if (!(f = fopen(nFichV, "ab+")))
    return ERROR;
  if (rand() < (RAND_MAX / 2))
    letra = 'N';
  else
    letra = 'Y';

  if (fwrite(&letra, sizeof(char), 1, f) == ERROR)
  {
    fclose(f);
    _error_in_voters();
    return ERROR;
  }
#ifdef DEBUG
  printf("v %ld: %c\n", (long)getpid(), letra);
#endif
  fclose(f);
  return OK;
}

/*Generates n_procs and writes their PID in a file*/
void create_sons(int n_procs, sem_t *semV, sem_t *semC, sem_t *semCTRL)
{
  int i;
  pid_t pid, *PIDs = NULL;
  FILE *fHijos;

  PIDs = (pid_t *)malloc(n_procs * sizeof(pid_t));
  if (!PIDs)
  {
    printf("Error allocating memory for PIDs\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < n_procs; i++)
  {
    if ((pid = (int)fork()) == 0) /*Function made for the sons*/
      voters(n_procs, semV, semC, semCTRL);
    else if (pid == ERROR)
    {
      if (i != 0)
        end_processes(i);
      kill(getpid(), SIGUSR1);
      return;
    }
    PIDs[i] = pid;
  }
  /*The writing of the PIDs in the file*/
  if (!(fHijos = fopen(NOMBREFICHERO, "wb")))
  {
    free(PIDs);
    fprintf(stderr, "ERROR opening the file %s to write\n", NOMBREFICHERO);
    end_processes(n_procs);
  }
  if (fwrite(PIDs, sizeof(pid_t), n_procs, fHijos) == ERROR)
    end_processes(n_procs);
  fclose(fHijos);

  free(PIDs);
}

/*Main function executed by the sons*/
void voters(int n_procs, sem_t *semV, sem_t *semC, sem_t *semCTRL)
{
  int i = 0, sig[NSIGNALS] = {SIGUSR1, SIGTERM, SIGUSR2, SIGINT};
  struct sigaction actSIG;
  sigset_t mask2, oldmask;
#ifdef DEBUG
  int aux;
#endif

  srand((int)getpid());

  /*Block temporarily SIGUSR1, SIGUSR2, SIGTERM and SIGINT to set their handers*/
  if (set_handlers(sig, NSIGNALS, &actSIG, &oldmask, _handler_voter) == ERROR)
  {
#ifdef DEBUG
    printf("Error en el set handler\n");
#endif
    _error_in_voters(n_procs);
  }
#ifdef TEST
  nanorandsleep();
#endif

  while (!got_sigUSR1) /*Suspend the process waiting for SIGUSR1*/
    sigsuspend(&oldmask);
  got_sigUSR1 = 0;

#ifdef DEBUG
  printf("Bucle principal de los votantes\n");
#endif
  while (1)
  { /*Main loop*/
#ifdef TEST
    nanorandsleep();
#endif

    /*Proposing a candidate*/
    if (down_try(semC) == ERROR)
    { /*Non candidate*/
#ifdef TEST
      nanorandsleep();
#endif
      while (!got_sigUSR2)
        sigsuspend(&oldmask);
      got_sigUSR2 = 0;
      /*Exclusion Mutua Votar*/
      while (down(semV) == ERROR)
        ;
#ifdef TEST
      nanorandsleep();
#endif
      if (votingCarefully(NOMBREVOTAR) == ERROR)
        _error_in_voters();

      if (up(semV) == ERROR)
        _error_in_voters();
    }
    else
    { /*Candidate*/
      candidato(n_procs, semCTRL);
#ifdef TEST
      nanorandsleep();
#endif
      /*Release the sem to choose a new candidate + send USR1 to start a new voting*/
      if (up(semC) == ERROR)
        _error_in_voters();

      if (send_signal_procs(SIGUSR1, n_procs, NO_PID) == ERROR)
        _error_in_voters();
    }
#ifdef DEBUG
    sem_getvalue(semCTRL, &aux);
    printf("PREUSR1 %d, %d\n", getpid(), aux);
#endif
#ifdef TEST
    nanorandsleep();
#endif
    while (!got_sigUSR1) /*Suspend the process waiting for SIGUSR1*/
      sigsuspend(&oldmask);
    got_sigUSR1 = 0;
#ifdef TEST
    nanorandsleep();
#endif
    if (got_sigTERM)
    {
      up(semCTRL);
#ifdef DEBUG
      printf("Hijo con PID=%ld sale por señal\n", (long)getpid());
#endif
      kill(getppid(), SIGUSR1);
      sem_close(semV);
      sem_close(semC);
      sem_close(semCTRL);
      exit(EXIT_SUCCESS);
    }
  }
}

/*Main function executed by the son candidato*/
void candidato(int n_procs, sem_t *semCTRL)
{
  int reading = 1, i, cont = 0, j;
  char *votes = NULL, result[15];
  FILE *f;

  if (!(votes = (char *)malloc((n_procs) * sizeof(char))))
  {
    send_signal_procs(SIGUSR2, n_procs, getpid());
    _error_in_voters();
    return;
  }

  /*Empty voting file*/
  if (!(f = fopen(NOMBREVOTAR, "wb")))
  {
    send_signal_procs(SIGUSR2, n_procs, getpid());
    _error_in_voters();
    return;
  }
  fclose(f);
#ifdef TEST
  nanorandsleep();
#endif
  /*Candidate votes*/
  if (votingCarefully(NOMBREVOTAR) == ERROR)
  {
    send_signal_procs(SIGUSR2, n_procs, getpid());
    _error_in_voters();
    return;
  }
  if (send_signal_procs(SIGUSR2, n_procs, getpid()) == ERROR)
  {
    _error_in_voters();
    return;
  }
#ifdef TEST
  nanorandsleep();
#endif
  /*Opens file to read the votes*/
  if (!fopen(NOMBREVOTAR, "rb"))
  {
    free(votes);
    _error_in_voters();
    return;
  }

  /*Read the votes every 0.1 seconds until everybody votes*/
  while (reading)
  {
#ifdef TEST
    nanorandsleep();
#endif

    if (down_try(semCTRL) == OK)
    {
#ifdef DEBUG
      printf("Falta un votante, finalización de votacion\n");
#endif
      up(semCTRL);
      fclose(f);
      return;
    }
    ournanosleep(WAITVOTE);
    fseek(f, 0, SEEK_SET);

    j = fread(votes, sizeof(char), n_procs, f);
    if (j == (n_procs))
      reading = 0;
    else if (j == ERROR)
    {
      _error_in_voters();
      return;
    }
#ifdef TEST
    nanorandsleep();
#endif
  }
  fclose(f);

  /*Print the result of the votation*/
  printf("Candidate %d => [", (int)getpid());
#ifdef TEST
  nanorandsleep();
#endif
  /*Count the votes*/
  for (i = 0; i < n_procs; i++)
  {
    if (votes[i] == 'Y')
      cont++;
    printf(" %c", votes[i]);
  }

  if (cont <= (n_procs / 2))
    printf(" ] => Rejected\n");
  else
    printf(" ] => Accepted\n");

  ournanosleep(WAITCANDIDATE);
  free(votes);
}