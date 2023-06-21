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

#include "funciones_minero.h"

#define INIC 0
#define MAX_THREADS 100
#define MAX POW_LIMIT - 1
#define WAIT MIL*MIL
#define MAX_TRY 50
#define N_SIGNALS_MIN 5


int encontrado;

volatile sig_atomic_t got_sigUSR1 = 0;
volatile sig_atomic_t got_sigUSR2 = 0;
volatile sig_atomic_t must_TERM = 0;

typedef struct
{
    long ep, eu, res;
} Entrada_Hash;

void init_block(Bloque *b, Wallet *sys_Wallet, long id, long obj);
int initMem(sem_t*sem,System_info **s_info);
void error_minero(sem_t *sem,char *str, int handler, int pipe, mqd_t *mq, System_info **s_info, int proc_index);
/*Si hay error se gestiona internamente y se realiza un exit()*/
void minero(int n_threads, int n_secs, int pid, int PipeEscr, sem_t *mutexSysInfo, sem_t *semMon)
{
    int obj, sol = -1, proc_index, sig[N_SIGNALS_MIN] = {SIGALRM, SIGUSR1, SIGTERM, SIGUSR2, SIGINT};
    System_info *s_info;
    struct sigaction actSIG;
    sigset_t MaskWithNoSignals, MaskWithSignals;
    struct mq_attr attributes;
    mqd_t mq;

    /*Cambio del manejador de señales*/
    if (set_handlers(sig, N_SIGNALS_MIN, &actSIG, &MaskWithSignals, &MaskWithNoSignals, _handler_minero) == ERROR)
        error_minero(mutexSysInfo,"Error setting the new signal handler", NO_PARAMETER, NO_PARAMETER, NULL, NULL, NO_PARAMETER);
#ifdef TEST
    nanorandsleep();
#endif
    /*Creará una cola de mensajes de capacidad SIZE*/
    attributes.mq_maxmsg = Q_SIZE;
    attributes.mq_msgsize = sizeof(Bloque);
    /*Conexión a la cola de mensajes*/
    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)ERROR)
        error_minero(mutexSysInfo," mq_open en Minero", NO_PARAMETER, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

    /*Creará una conexión a memoria para la comunicación de información del sistema :n_mineros, ... */
    down(mutexSysInfo);
    proc_index = initMem(mutexSysInfo,&s_info);
    /*Registrar sus datos en el sistema (PIDs ,n_mineros...)*/
    wallet_set_pid(&(s_info->Wallets[proc_index]), getpid());
    wallet_set_coins(&(s_info->Wallets[proc_index]), 0);
    up(mutexSysInfo);

#ifdef DEBUG
    fprintf(stderr, "\033[0Mi pid es %ld y mi proc_index %d\n", (long)getpid(), proc_index);
#endif

    while (!got_sigUSR1)
        sigsuspend(&MaskWithNoSignals);
    got_sigUSR1 = 0;

    down((mutexSysInfo));
    s_info->n_mineros++;
    up((mutexSysInfo));
#ifdef TEST
    nanorandsleep();
#endif
    down(&(s_info->MutexBAct));
    wallet_set_pid(&(s_info->BloqueActual.Wallets[proc_index]), getpid());
    up(&(s_info->MutexBAct));
    got_sigUSR2 = 0;

    /*Bucle de minería y mandar las soluciones hasta que nos llegue un alarm de n_secs o un SIGINT*/
    while (!must_TERM)
    {
#ifdef TEST
    nanorandsleep();
#endif
        /*Definición local de objetivo*/
        obj = s_info->BloqueActual.obj;
        /*Salvo en la primera se debe enviar el bloque de la ronda anterior al proceso Registrador*/
        if (s_info->UltimoBloque.id != -1)
        {
            if (write(PipeEscr, &(s_info->UltimoBloque), sizeof(Bloque)) == -1)
                perror("Error mandando un paquete Registrador");
        }
        /*Ronda de minado:*/
        sol = minar(n_threads, obj);
#ifdef DEBUG
        fprintf(stderr, "\033[0;34mHe minado %d->%d\n", obj, sol);
#endif
#ifdef TEST
    nanorandsleep();
#endif

        /*Tras salir de la minería se elige a un ganador de forma que evite las condiciones de carrera (semáforo)*/
        if (sol != -1)
        {
            if (down_try(&(s_info->primer_proc)) != ERROR)
            { /*El primero se proclama proceso: Ganador*/
                ganador(s_info, obj, sol, proc_index, mutexSysInfo,semMon, mq);
                up(&(s_info->primer_proc));
            }
            else /*El resto procesos: Perdedor*/
                perdedor(s_info, &MaskWithNoSignals, proc_index);
        }
        else /*El resto procesos: Perdedor*/
            perdedor(s_info, &MaskWithNoSignals, proc_index);

        while (!got_sigUSR1 && !must_TERM)
            sigsuspend(&MaskWithNoSignals);
        got_sigUSR1 = 0;

        if (sigprocmask(SIG_UNBLOCK, &MaskWithSignals, NULL) == ERROR)
            error_minero(mutexSysInfo,"Error desbloqueando las señales", NO_PARAMETER, PipeEscr, &mq, &s_info, proc_index);
    }

#ifdef DEBUG
    fprintf(stderr, "\033[0;31mMe voy:%d\n", getpid());
#endif
#ifdef TEST
    nanorandsleep();
#endif
    /*Si es el último minero se libera todo y se envia al monitor (si está activo) el cierre del sistema*/
    down(mutexSysInfo);
    s_info->n_mineros--;
    if (s_info->n_mineros == 0)
    {
        s_info->BloqueActual.id = -1;
        if (mq_send(mq, (char *)(&(s_info->BloqueActual)), sizeof(Bloque), 0) == ERROR) /*Si falla no lo consideramos un errror fatal*/
            perror("Error mandando el mensaje de finalización al monitor");

#ifdef DEBUG
        fprintf(stderr, "\033[0Soy el ultimo:%d\n", getpid());
#endif
        sem_destroy(&(s_info->primer_proc));
        sem_destroy(&(s_info->MutexBAct));
        shm_unlink(SHM_NAME);
        mq_unlink(MQ_NAME);
        sem_unlink(SEM_NAME_MIN);
        sem_unlink(nameSemNmin); /*Puede dar error en caso extremo de  que uno se una un par de líneas más arriba*/
    }
    else
        wallet_set_pid(&(s_info->Wallets[proc_index]), 0);
    up(mutexSysInfo);

    close(PipeEscr);
    if (munmap(s_info, sizeof(System_info)) == ERROR)
        error_minero(mutexSysInfo,"munmmap en Minero", NO_PARAMETER, NO_PARAMETER, &mq, NULL, NO_PARAMETER);
    
    sem_close(semMon);

    if (mq_close(mq) == ERROR)
        error_minero(mutexSysInfo,"mq_close en Minero", NO_PARAMETER, NO_PARAMETER, &mq, NULL, NO_PARAMETER);
}

void ganador(System_info *sys, int obj, int sol, int proc_index, sem_t *mutexSysInfo, sem_t *semMon, mqd_t mq)
{
    int i;

#ifdef DEBUG
    fprintf(stderr, "\033[0Inic ganador %d\n", getpid());
#endif
    /*Prepara memoria compartida para la votación introduciendo la solución en el bloque actual
    e inicializando el listado de votos*/

    /*Preparación de votación*/
    down(&(sys->MutexBAct)); /*Revisar*/
    down(mutexSysInfo);
#ifdef TEST
    nanorandsleep();
#endif
    sys->BloqueActual.sol = sol;
    sys->BloqueActual.votos_a = 1;
    sys->BloqueActual.n_votos = 1;
    sys->BloqueActual.n_mineros = sys->n_mineros;

    sys->BloqueActual.pid = getpid();

    /*Enviar la señal SIGUSR2 indicando que la votación puede arrancar*/
#ifdef DEBUG
    fprintf(stderr, "\033[0;34mGanador envía SIGUSR2");
#endif
    send_signals_miners(sys->Wallets, proc_index, SIGUSR2);
#ifdef TEST
    nanorandsleep();
#endif
    sys->Votes_Min[proc_index] = 1;
    up(mutexSysInfo);
    up(&(sys->MutexBAct));

    /*Realiza esperas no activas cortas hasta que todos los procesos hallan votado o transcurra
       el máximo numero de intentos(podemos calentarnos y intentar quitar eso)*/
    for (i = 0; i < MAX_TRY; i++)
    {
#ifdef DEBUG
        fprintf(stderr, "\033[0Intento:%d %ld\n", i, sys->n_mineros);
#endif
        ournanosleep(WAIT);
        if (sys->BloqueActual.n_mineros == sys->BloqueActual.n_votos)
            break;
    }
#ifdef TEST
    nanorandsleep();
#endif
    /*Una vez terminada la votación se comprueba el resulatado y se introduce el bloque correspondiente de memoria */
    /* Si se aceptado recibe una moneda ( aumenta el contador )*/
    down(mutexSysInfo);
    down(&(sys->MutexBAct));
    if (sys->BloqueActual.votos_a >= sys->BloqueActual.n_votos / 2)
    {
        wallet_set_coins(&(sys->BloqueActual.Wallets[proc_index]), wallet_get_coins(&(sys->BloqueActual.Wallets[proc_index])) + 1);
        wallet_set_coins(&(sys->Wallets[proc_index]), wallet_get_coins(&(sys->Wallets[proc_index])) + 1);
    }
    #ifdef TEST
    nanorandsleep();
#endif
    up(&(sys->MutexBAct));
    up(mutexSysInfo);
    /*Ahora se debe enviar el bloque resuelto por cola de mensajes al monitor si hay alguno activo*/
    /* Se deber´an implementar los mecanismos adecuados para garantizar que el envío se realiza si y solo si
    el monitor est´a esperando para recibirlo, por ejemplo a trav´es de un semaforo con nombre.*/
    down(&(sys->MutexBAct));
    /*Manda a la cola de mensajes el bloque actual*/
    if(down_try(semMon)!=ERROR){
        up(semMon);
        if (mq_send(mq, (char *)(&(sys->BloqueActual)), sizeof(Bloque), 0) == ERROR) /*Un fallo en send no lo consideramos un error que deba acabar con el sistema*/
            perror(" Error in mq_send");
    }
        
#ifdef TEST
    nanorandsleep();
#endif

    up(&(sys->MutexBAct));
#ifdef TEST
    nanorandsleep();
#endif
    /*Prepara la siguiente ronda de minería*/
    down(mutexSysInfo);
    down(&(sys->MutexBAct));

    copy_block(&(sys->UltimoBloque), &(sys->BloqueActual));
    init_block(&(sys->BloqueActual), sys->Wallets, sys->UltimoBloque.id + 1, sol);
#ifdef TEST
    nanorandsleep();
#endif
    up(&(sys->MutexBAct));
    /*Reiniciar la minería con el envio de un SIGUSR1 a todos los mineros*/

#ifdef DEBUG
    fprintf(stderr, "\033[0;32mGanador envía SIGUSR1\n");
#endif
    send_signals_miners(sys->Wallets, NO_PARAMETER, SIGUSR1);
    up(mutexSysInfo);

#ifdef DEBUG
    fprintf(stderr, "\033[0Fin ganador %d\n", getpid());
#endif
    return;
}

/*Debemos elegir si el error se gestiona aquí o devolvemos error*/
void perdedor(System_info *sys, sigset_t *oldmask, int index_proc)
{
#ifdef DEBUG
    fprintf(stderr, "\033[0 Inic perdedor %d\n", getpid());
#endif
#ifdef TEST
    nanorandsleep();
#endif
    /*Si no le ha llegado aún la señal SIGUSR2 se espera a que la votación este lista*/
    while (!got_sigUSR2)
        sigsuspend(oldmask);
    got_sigUSR2 = 0;
    /*Exclusion Mutua Votar*/
    down(&(sys->MutexBAct));
#ifdef TEST
    nanorandsleep();
#endif
#ifdef DEBUG
    fprintf(stderr, "\033[0 Perdedor vota %d\n", getpid());
#endif
    /*Una vez recibido comprueba el bloque actual y registra su voto en función de si la solución es correcta*/
    sys->BloqueActual.n_votos++;
    sys->Votes_Min[index_proc] = 0;
    if (pow_hash(sys->BloqueActual.sol) == sys->BloqueActual.obj)
    {
        sys->BloqueActual.votos_a++;
        sys->Votes_Min[index_proc] = 1;
    }

    up(&(sys->MutexBAct));
#ifdef DEBUG
    fprintf(stderr, "\033[0Fin perdedor %d\n", getpid());
#endif
    /*Entra en una espera no activa (Se realizará fuera de la función) hasta la siguiente votación*/
}

void registrador(int PipeLect)
{
    int nbytes = 1, fd;
    Bloque bloque;
    char Filename[WORD_SIZE];
    int sig[1]={SIGINT}; 
    struct sigaction actSig;
    sigset_t signals, no_signals;
    #ifdef DEBUG

    printf("[%d] Printing blocks...\n", getpid());
    #endif
    /*Ignoring sigint*/
    set_handlers(sig,1,&actSig,&signals,&no_signals, NULL);

    sprintf(Filename, "reg%d.dat", getppid());

    if (!(fd = open(Filename, O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0666)))
        error("Error en registrador al abrir el fichero\n");

    while (nbytes) /*Se ejecuta siempre que haya escritores*/
    {
        if ((nbytes = read(PipeLect, &bloque, sizeof(Bloque))) == -1)
        {
            close(fd);
            close(PipeLect);
            error("Error reading bloque");
        }

        /*Mientras las tuberías esten abiertas: se leen bloques usando dprintf (impresión en descriptores de ficheros)
     en un archivo que dependa del pid del padre (Minero)*/
        print_bloque(fd, &bloque);
    }

    close(fd);
    close(PipeLect);
    #ifdef DEBUG
    printf("REGISTRADOR SE VA porque no hay escritores\n");
    #endif

    exit(EXIT_SUCCESS);
}

void init_block(Bloque *b, Wallet *sys_Wallets, long id, long obj)
{
    int i;

    if (b == NULL || !sys_Wallets)
        return;

    b->id = id;
    b->obj = obj;

    for (i = 0; i < MAX_MINERS; i++)
        copy_wallet(&(b->Wallets[i]), &(sys_Wallets[i]));
}

void _handler_minero(int sig)
{
    switch (sig)
    { 
    case SIGINT:
    #ifdef DEBUG
        printf("\033[0;31m ME LLEGÓ SIGINT \n");
#endif
    case SIGALRM:
    case SIGTERM:

#ifdef DEBUG
        printf("\033[0;31m ME LLEGÓ Finalización\n");
#endif
        
        must_TERM = 1;
        break;

    case SIGUSR1:
#ifdef DEBUG
        printf("\033[0;32m USR1 %ld\n", (long)getpid());
#endif
        got_sigUSR1 = 1;
        break;

    case SIGUSR2:
#ifdef DEBUG
        printf("\033[0;34m USR2 %ld\n", (long)getpid());
#endif
        got_sigUSR2 = 1;
        break;

    default:
#ifdef DEBUG
        printf("SIGNAL UNKNOWN(%d) %ld\n", sig, (long)getpid());
#endif
        break;
    }
}

long minar(int n_threads, int obj)
{
    int i, incr = ((int)MAX) / n_threads;
    long solucion = -1;
    Entrada_Hash t[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    void *sol[MAX_THREADS];

    encontrado = 0;
    /*Creación de hilos*/
    for (i = 0; i < n_threads; i++)
    {
        t[i].ep = incr * i;
        if (i == n_threads - 1)
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
    for (i = 0; i < n_threads; i++)
    {
        if (pthread_join(threads[i], sol + i))
        {
            for (i++; i < n_threads; i++)
                (void)pthread_join(threads[i], sol + i);
            perror("pthread_join en la unión de hilos"); /*Un error en un join de un hilo no lo consideramos un error fatal*/
        }
        if ((long)sol[i] != -1)
            solucion = (long)sol[i];
    }

    return (long)solucion;
}

void *func_minero(void *arg)
{
    long i;
    Entrada_Hash *e = (Entrada_Hash *)arg;

    for (i = e->ep; (i < e->eu) && (encontrado == 0) && (got_sigUSR2 == 0); i++) /*Si se recibe una SIGUSR2 u otro hilo halla una solución significa que un proceso a encontrado la solución*/
        if (pow_hash(i) == e->res)
        {
            encontrado = 1;
            pthread_exit((void *)i);
        }

    pthread_exit((void *)(-1));
}

int initMem(sem_t *mutex,System_info **s_info)
{ /*CORREGIR, en caso de error se lleva el down del mutexSysInfo*/
    int proc_index;
    int handler_info_sist;

    if ((handler_info_sist = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) != ERROR) /*Si la memoria no está creada*/
    {
#ifdef DEBUG
        printf("Creación de memoria\n");
#endif
        /*INICIALIZACIÓN DE LA MEMORIA COMPARTIDA*/

        /*Asignación de tamaño*/
        if (ftruncate(handler_info_sist, sizeof(System_info)) == ERROR) /*CORREGIR, en caso de error se lleva el down del mutexSysInfo*/
            error_minero(NULL,"Error truncating the shared memory segment info_sist", handler_info_sist, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

        /*Enlazamiento al espacio de memoria*/
        if (((*s_info) = (System_info *)mmap(NULL, sizeof(System_info), PROT_READ | PROT_WRITE, MAP_SHARED, handler_info_sist, 0)) == MAP_FAILED)
            error_minero(NULL,"Error mapping the shared memory segment info_sist", handler_info_sist, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

        /*INICIALIZACIÓN DE LA MEMORIA COMPARTIDA*/
        /*Inicialización de los semáforos*/
        if (sem_init(&((*s_info)->primer_proc), SHARED, 1) == ERROR)
            error_minero(NULL,"sem_open primer_proc", handler_info_sist, NO_PARAMETER, NULL, NULL, NO_PARAMETER);
        if (sem_init(&((*s_info)->MutexBAct), SHARED, 1) == ERROR)
            error_minero(NULL,"sem_open MutexBact", handler_info_sist, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

        /*Inicialización de las Wallets y los votos*/
        for (proc_index = 0; proc_index < MAX_MINERS; proc_index++)
            wallet_set_pid(&((*s_info)->Wallets[proc_index]), 0);
        proc_index = 0;

        (*s_info)->n_mineros = 0;
        (*s_info)->BloqueActual.id = 0;
        (*s_info)->BloqueActual.obj = INIC;
        (*s_info)->UltimoBloque.id = -1;
        
        got_sigUSR1 = 1;
    }
    else /*Si no es el primero simplemente abre el segmento, lo enlaza. Debe garantizarse que el sistema esta listo (Semáforo)*/
    {
#ifdef DEBUG
        printf("Conectandose a la memoria\n");
#endif
        if ((handler_info_sist = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == ERROR) /*Control de errores*/
            error_minero(mutex,"Error opening the shared memory segment info_sist", NO_PARAMETER, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

        if (((*s_info) = (System_info *)mmap(NULL, sizeof(System_info), PROT_READ | PROT_WRITE, MAP_SHARED, handler_info_sist, 0)) == MAP_FAILED)
            error_minero(mutex,"Error mapping the shared memory segment info_sist", handler_info_sist, NO_PARAMETER, NULL, NULL, NO_PARAMETER);

        /*Looks for the first empty space*/
        for (proc_index = 0; proc_index < MAX_MINERS && wallet_get_pid(&((*s_info)->Wallets[proc_index])) != 0; proc_index++)
            ;

        if (proc_index == MAX_MINERS) /*En caso de que ya halla el máximo de mineros*/
            error_minero(mutex,"Number of miners reached the maximum", handler_info_sist, NO_PARAMETER, NULL, s_info, proc_index);
    }

    close(handler_info_sist);
    return proc_index;
}

void error_minero(sem_t *mutex,char *str, int handler, int pipe, mqd_t *mq, System_info **s_info, int proc_index)
{
    if (str)
        perror(str);

    if (handler != NO_PARAMETER)
        close(handler);

    if (pipe != NO_PARAMETER)
        close(pipe);

    if (mq != NULL)
        mq_close(*mq);

    if(mutex)
        up(mutex);
    if (s_info != NULL)
    {
        if(mutex){
           down(mutex);
            (*s_info)->n_mineros--;
            up(mutex); 
        }
        if(proc_index!=NO_PARAMETER)
            wallet_set_pid(&((*s_info)->Wallets[proc_index]),0);
        (void)munmap(s_info, sizeof(System_info));
    }

    exit(EXIT_FAILURE);
}
