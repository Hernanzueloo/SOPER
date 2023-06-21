#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>

#define POW_LIMIT 99997669
#define MAX POW_LIMIT - 1
#define CONT 23

int padre ( int Escritura, int Lectura){
    int k = 0,m, nbytes, status;
    while ( 1){
        nbytes = write ( Escritura, &k,sizeof(int));
     if (nbytes == -1)
        {
            perror("read");
            close(Escritura);
            wait(&status);
            printf("Son exited with status %d\n",status);

            close(Lectura);
            exit(EXIT_FAILURE);
        }
        if (nbytes==0){
            close(Escritura);
            wait(&status);
            printf("Son exited with status %d\n",status);
            close(Lectura);
            exit(EXIT_FAILURE);
        }
        nbytes = read(Lectura, &k, sizeof(int)*1);
        if (nbytes == -1)
        {
            perror("read");
            close(Escritura);
            wait(&status);
            printf("Son exited with status %d\n",status);
            close(Lectura);
            exit(EXIT_FAILURE);
        }
        if (nbytes==0){

            perror("read");
            close(Escritura);
            wait(&status);
            printf("49: %d Son exited with status %d\n",m,status);
            
            close(Lectura);
            exit(EXIT_FAILURE);
        }
         k+=1;

        printf ( "PADRE :  %d \n", k);

        if ( k==2*CONT){
            close(Escritura);
            wait(&status);
            printf(" Son exited with status %d\n",status);
            close(Lectura);
            return k;
        }
    }
}

int hijo (int Escritura, int Lectura){
    int k = 1,m, nbytes;

    while (1){

    
    nbytes = read(Lectura, &k, sizeof(int)*1);
        if (nbytes == -1)
        {
            perror("read");
            close(Escritura);
            close(Lectura);
            if( m == 30) exit(EXIT_SUCCESS);

            exit(EXIT_FAILURE);
        }
        if (nbytes==0){
            perror("readv");
            close(Escritura);            
            close(Lectura);
            if ( k == 2*CONT -1){

                 exit(EXIT_SUCCESS);
            }
            exit(EXIT_FAILURE);
        }
    k+=1;
    printf ("HIJO %d \n", k);
    nbytes = write ( Escritura, &k,sizeof(int));
     if (nbytes == -1)
        {
            perror("read");
            close(Escritura);
            close(Lectura);

            exit(EXIT_FAILURE);
        }
        if (nbytes==0){
            close(Escritura);
            close(Lectura);

            exit(EXIT_FAILURE);
        }
    }
}

int main(){
    int i,K,t1[100],busq=17,status,Pipe1[2],Pipe2[2];
    int *sol[100];
    int a=0;
    pid_t pid;
  

    status = pipe(Pipe1);
    if (status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    status = pipe(Pipe2);
    if (status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
   
    pid = fork();

    if ( pid){
        close(Pipe1[0]);
        close(Pipe2[1]);
       a = padre (Pipe1[1], Pipe2[0]);
    }else{
         close(Pipe1[1]);
        close(Pipe2[0]);
        hijo (Pipe2[1], Pipe1[0]);
    }

    printf ( " a = %d, %d\n" , a, a==2*CONT);
     return EXIT_SUCCESS;
}