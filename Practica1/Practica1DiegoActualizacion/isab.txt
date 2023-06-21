#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pow.h"

#define MAX POW_LIMIT

typedef struct
{
    long ep, eu, res;
} entradaHash;

void *func_minero(void *arg) {
    int i;
    long x=-1;
    entradaHash *e=(entradaHash*)arg;

    for (i = e->ep; (i < e->eu) && (x <= 0); i++)
    {
        if ((long)pow_hash(i) == e->res)
        {
            x = i;
        }
    }

   pthread_exit((void *)x);
}

int main(int argc, char *argv[]) {
   pthread_t thread1, thread2;
   int rc, rc2, t1, t2;
   entradaHash t;
   void *sol, *sol2;

    t1=clock();
/*Creacion del hilo 1*/
    t.ep = 0;
    t.eu = MAX/2;
    t.res = (long)pow_hash(MAX/2+1000);

   rc = pthread_create(&thread1, NULL, func_minero, (void *)&t);
   if (rc) {
      printf("Error creating thread 1\n");
      exit(-1);
   }
/*Creacion del hilo 2*/
    t.ep = MAX/2+1;
    t.eu = MAX;
    t.res = (long)pow_hash(MAX/2+1000);

   rc = pthread_create(&thread2, NULL, func_minero, (void *)&t);
   if (rc) {
      printf("Error creating thread 1\n");
      exit(-1);
   }


   rc = pthread_join(thread1, &sol);
   if (rc) {
      printf("Error joining thread 1\n");
      exit(-1);
   }

   rc2 = pthread_join(thread2, &sol2);
   if (rc2) {
      printf("Error joining thread 2\n");
      exit(-1);
   }
   t2=clock();

   printf("Thread 1 returned value: %ld\n   Time spent: %d", (long)sol, t2-t1);
   printf("\nThread 2 returned value: %ld\n   Time spent: %d", (long)sol2, t2-t1);

   return 0;
}