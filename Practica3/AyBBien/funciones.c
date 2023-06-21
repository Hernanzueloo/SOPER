/**
 * @file funciones.h
 * @author Diego Rodríguez y Alejandro García
 * @brief 
 * @version 
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

#include "funciones.h"

#define FILE_NAME "mrush_sequence.dat"
#define QUEUE_SIZE 6
#define N_SIZE 4
#define OBJ 0
#define SOL 1
#define RES 2
#define FIN 3


struct _Shm_struct
{
    sem_t sem_vacio, sem_mutex, sem_lleno;
    long num[N_SIZE][QUEUE_SIZE];
};

STATUS comprobador(int fd_shm, int lag, sem_t *semCtrl){
    FILE *f;
    long new_objetivo,new_solucion, obj,sol;
    int fin=0,res, index = 0; /*Index apunta a la primera direccción vacía*/
    Shm_struct *mapped;
    
    /*Mapeará el segmento de memoria*/
    if(ftruncate(fd_shm,sizeof(Shm_struct))==ERROR){
        perror("ftruncate");
        close(fd_shm);
        /*Comunicar de alguna forma a los otro proceso que fin*/
        return ERROR;
    }
    #ifdef DEBUG
        printf("Pasa ftruncate %d\n", getpid());
    #endif
    if((mapped = (Shm_struct *)mmap(NULL,sizeof(Shm_struct),PROT_READ | PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0) )== MAP_FAILED){
        perror("mmap");
        close(fd_shm);
        /*Comunicar de alguna forma a los otro proceso que fin*/
        return ERROR;
    }
    #ifdef DEBUG
        printf("Pasa mmap %d \n", getpid());
    #endif
    /*Cerramos el descriptor ya que tenemos un puntero a la "posición" de mamoria compartida*/
    close(fd_shm);
    /*Inicialización de los semaforos*/

    if( ERROR== sem_init(&(mapped->sem_mutex),2,1)){
        perror("Sem_init mutex");
        /*Comunicar de alguna forma a los otro proceso que fin*/
        munmap(mapped, sizeof(Shm_struct));
        return ERROR;
    }
    #ifdef DEBUG
        printf("Pasa inicializa sem mutex %d\n", getpid());
    #endif
    if( ERROR == sem_init(&(mapped->sem_vacio),2,QUEUE_SIZE)){
        perror("Sem_init vacio");
        sem_destroy(&mapped->sem_mutex);
        /*Comunicar de alguna forma a los otro proceso que fin*/
        munmap(mapped, sizeof(Shm_struct));
        return ERROR;
   }
   #ifdef DEBUG
        printf("Pasa inicializa sem vacio %d\n", getpid());
    #endif
    if( ERROR == sem_init(&(mapped->sem_lleno),2,0)){
        perror("Sem_init lleno");
        sem_destroy(&mapped->sem_vacio);
        sem_destroy(&mapped->sem_mutex);
        munmap(mapped, sizeof(Shm_struct));
        /*Comunicar de alguna forma a los otro proceso que fin*/
        return ERROR;
    }
    #ifdef DEBUG
        printf("Pasa inicializa sem _lleno %d\n", getpid());
        sleep(2);
    #endif
    /*Cerramos el semáforo que ya no utilizaremos*/
    up(semCtrl);
    sem_close(semCtrl);
    /*TEMPORAL*/ f = fopen(FILE_NAME,"r");
    /*TEMPORAL*/fscanf(f,"%ld", &sol);
    /*TEMPORAL*/
    while(1){
        /*Recibirá un bloque a través de mensajes por parte del minero*/

        /*TEMPORAL :Momentaneamente para probar la implementación de monitor-comprobador lo leeremos del fichero proporcionado*/
        obj = sol;
        fscanf(f,"%ld", &new_solucion);
        sol =new_solucion;
        if(sol== obj){
            /*Forma de decir que se ha acabado el fichero del que leíamos*/
            #ifdef DEBUG
            printf("FIN %ld -> %ld %ld fin: %d\n",obj,sol, pow_hash(sol),fin);
            #endif
            fin = 1;
        }
        /*TEMPORAL:Aquí acaba lo  temporal*/
        
        /*Comproborá con la función hash si es correcta y añadirá una bandera que indique si es correcta o no*/
        if( obj == pow_hash(sol)){
            res = 1; /*Respuesta correcta*/
        }else{
            res = 0; /*Respuesta incorrecta*/
        }
        #ifdef DEBUG
            printf("Comprueba %ld -> %ld %ld fin: %d\n",obj,sol, pow_hash(sol),fin);
        #endif
        /*Se la trasladará al monitor a través de la memoria compartida*/
        /* Esquema de productro-consumidor (El es el productor)*/
        down(&mapped->sem_vacio);
        down(&mapped->sem_mutex);
        mapped->num[OBJ][index] = obj;
        mapped->num[SOL][index] = sol;
        mapped->num[RES][index] = res;
        mapped->num[FIN][index]=fin;
        index=(index+1)%QUEUE_SIZE;
        up(&mapped->sem_mutex);
        up(&mapped->sem_lleno);


        /*Realizará una espera de <lag> milisegundos */
        ournanosleep(lag*MIL);
         /*Cuando se reciba dicho bloque lo introducirá en memoria compartida para avisar a Monitor de la fincalización*/
        if(fin)
            break;
        /*Repitirá el proceso hasta que reciba (de minero un bloque especial de finalización)*/
    }
    #ifdef DEBUG
            printf("Comprobador sale %d\n", getpid());
    #endif
    /*Liberación de recursos y fin*/
    /*TEMPORAL*/fclose(f);
    #ifdef DEBUG
        sleep(1);
    #endif
    if(munmap(mapped, sizeof(Shm_struct))==ERROR){
        perror("munmap comprobador\n");
        return ERROR;
    }
    return OK;
}

STATUS monitor(int fd_shm, int lag, sem_t *semCtrl){
    long obj,sol;
    int fin=0,res, index = 0; /*Index apunta a la primera direccción llena*/
    Shm_struct *mapped;

    /*Mapeará el segmento de memoria*/
    if((mapped = (Shm_struct *) mmap(NULL,sizeof(Shm_struct),PROT_READ | PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0) )== MAP_FAILED){
        perror("mmap");
        close(fd_shm);
        /*Comunicar de alguna forma a los otro proceso que fin*/
        return ERROR;
    }
    /*Cerramos el descriptor ya que tenemos un puntero a la "posición" de mamoria compartida*/
    close(fd_shm);
    down(semCtrl);
    sem_close(semCtrl);
    while(1){
        /*Extaerá un bloque realizando de consumidor en el esquema productor-consumidor*/
        down(&mapped->sem_lleno);
        down(&mapped->sem_mutex);
        obj=mapped->num[OBJ][index];
        sol=mapped->num[SOL][index];
        res=mapped->num[RES][index];
        fin=mapped->num[FIN][index];
        index=(index+1)%QUEUE_SIZE;
        up(&mapped->sem_mutex);
        up(&mapped->sem_vacio);

        if(fin){
            #ifdef DEBUG
            printf("MON FIN %ld -> %ld %ld fin: %d\n",obj,sol, pow_hash(sol),fin);
            #endif
            break;
        }
            
        /*Mostrará por pantalla el resultado con la siguiente sintaxis*/
        /*Solution accepted: %08ld --> %08ld o Solution rejected: %08ld !-> %08ld (Siendo el promer numero el objetivo y el segundo la solución)*/
        if(res){
            printf("Solution accepted: %08ld --> %08ld\n", obj, sol);
        }else{
            printf("Solution rejected: %08ld !-> %08ld\n", obj, sol);
        }
        /*Realizará una espera de <lag> milisegundos */
         ournanosleep(lag*MIL);
        /*Repetirá el ciclo de extracción y muestra hasta recibir un bloque especial que indique la finalización del sistema*/
    }
    /*Liberación de recursos y fin*/
    /*Cerramos los recursos abiertos ya que han sido abiertos por el comprobador y nosotros somos los últimos en usarlos*/
    sem_destroy(&mapped->sem_vacio);
    sem_destroy(&mapped->sem_mutex);
    sem_destroy(&mapped->sem_lleno);
    munmap(mapped, sizeof(Shm_struct));
    return OK;
}


/*Estructura productor-consumidor recomenda: */

/*Productor {                            • Se garantizara que no se pierde ningun bloque con independencia del retraso que se introduzca en cada proceso.
    Down ( sem_empty ) ;
    Down ( sem_mutex ) ;                 • Se usara un buffer circular de 6 bloques en el segmento de memoria compartida
    A ~n adirElemento () ;
    Up ( sem_mutex ) ;                   • Se alojaran en el segmento de memoria compartida tres semaforos sin nombre para implementar el algoritmo de productor–consumidor.
    Up ( sem_fill ) ;
}
Consumidor {
    Down ( sem_fill ) ;
    Down ( sem_mutex ) ;
    ExtraerElemento () ;
    Up ( sem_mutex ) ;
    Up ( sem_empty ) ;
}
*/