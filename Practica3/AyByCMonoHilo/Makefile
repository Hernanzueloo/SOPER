CC=gcc 
CFLAGS = -ansi -pedantic -g -Wall -ggdb 


DFLAGS = -D ##DEBUG
IFLAGS=-I. ##La opción -I se usa para indicar donde se encuentran los ficheros cabeceras (.h)

LIBS= -lm -lpthread -lrt ### -lm enlaza la biblioteca matematica, -pthread enlaza la biblioteca de hilos

MON_OBJS = monitor.o funciones.o utils.o
MIN_OBJS = minero.o pow.o

## EXECUTIVES CREATION ##
#----------------------------------------------------------------------------------------------------------------------------

all: monitor minero

monitor: monitor.o funciones.o utils.o pow.o
	$(CC) -o $@ $^ $(LIBS)

	
monitor.o: monitor.c funciones.h utils.h pow.h
	$(CC) -c $< 

funciones.o: funciones.c funciones.h utils.h pow.h
	$(CC) -c $< 

utils.o: utils.c utils.h
	$(CC) -c $< 


minero: minero.o pow.o utils.o
	$(CC) -o $@ $^ $(LIBS)

minero.o: minero.c pow.h utils.h
	$(CC) -c $< 

pow.o: pow.c pow.h
	$(CC) -c $< 
#----------------------------------------------------------------------------------------------------------------------------
run_all: ./monitor 1000 ./monitor 500 ./minero 200 100  #Es posible que esto haga que se ejecuten en orden, no simultaneamente

run_mon:
	./monitor 100

run_monv:
	valgrind --leak-check=full --show-leak-kinds=all ./monitor 2 10

run_min:
	./monitor 10 5

run_minv:
	valgrind --leak-check=full --show-leak-kinds=all ./monitor 2 10
#----------------------------------------------------------------------------------------------------------------------------
clean:
	rm -v monitor minero *.o

clean_mon:
	rm -v monitor  $(MON_OBJS)

clean_min:
	rm -v  minero $(MIN_OBJS)
cs:
	ls /dev/shm

rs:
	 rm /dev/shm/shm*
	 rm /dev/shm/sem*