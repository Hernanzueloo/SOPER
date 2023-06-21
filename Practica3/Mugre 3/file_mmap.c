/* READER */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define FILE_NAME "/test"
#define SEM_NAME "/sem1"
#define FILE_NAM "mrush_sequence.dat"

typedef struct 
{
  sem_t sem;
  int mem;
}StructLO;

int main(void) {
  long created = 0,objetivo,solucion;
  int fd,i=0;
  StructLO *mapped = NULL;
  FILE * f;
  sem_t *sem;
  struct timespec t = {0,12000000};
  /*Creation of the semaphores (to vote, to select the candidate and to control)*/

  if ((fd =shm_open(FILE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
    if (errno == EEXIST) {
      if ((fd = shm_open(FILE_NAME, O_RDWR, 0)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
    } else {
      perror("open");
      exit(EXIT_FAILURE);
    }
  } else {
    if (ftruncate(fd, sizeof(StructLO)) == -1) {
      perror("ftruncate");
      close(fd);
      exit(EXIT_FAILURE);
    }
    created = 1;
  }
  f = fopen(FILE_NAM,"r");
    
  if ((mapped = (StructLO *)mmap(NULL, sizeof(StructLO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    close(fd);
    exit(EXIT_FAILURE);
  }
  if(created){
    if( -1 == sem_init(&mapped->sem,2,1)){
    perror("Sem_init");
    close(fd);
    exit(EXIT_FAILURE);
   }
   mapped->mem = 0;
   
  }

  close(fd);
  while(i<1000){
    sem_wait(&mapped->sem);
    mapped->mem += 1;
    fscanf(f,"%d", &objetivo);
    fscanf(f,"%d", &solucion);
    printf("Counter: %d %d %d %d\n",getpid(),objetivo,solucion, mapped->mem);
    sem_post(&mapped->sem);
    i++;
    nanosleep(&t,NULL);
  }
  fclose(f);
  sem_destroy(&mapped->sem);
  munmap(mapped, sizeof(StructLO));
  shm_unlink ( FILE_NAME);
  exit(EXIT_SUCCESS);
}