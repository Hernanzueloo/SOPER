CC=gcc 
CFLAGS = -g -Wall -ggdb 


DFLAGS = -DDEBUG
IFLAGS=-I. ##La opción -I se usa para indicar donde se encuentran los ficheros cabeceras (.h)

LIBS= -lm -lpthread -lrt ### -lm enlaza la biblioteca matematica, -pthread enlaza la biblioteca de hilos

MON_OBJS = monitor.o funciones.o utils.o
MIN_OBJS = minero.o pow.o

## EXECUTIVES CREATION ##
#----------------------------------------------------------------------------------------------------------------------------

all: monitor minero

monitor: monitor.o funciones_monitor.o utils.o pow.o
	$(CC) -o $@ $^ $(LIBS)

monitor.o: monitor.c utils.h pow.h
	$(CC) $(CFLAGS) -c $< 

funciones_monitor.o: funciones_monitor.c funciones_monitor.h utils.h pow.h
	$(CC) $(CFLAGS) -c $< 

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $< 

minero: minero.o funciones_minero.o pow.o utils.o
	$(CC) -o $@ $^ $(LIBS)

minero.o: minero.c funciones_minero.h pow.h utils.h
	$(CC) $(CFLAGS) -c $< 

funciones_minero.o: funciones_minero.c funciones_minero.h pow.h utils.h
	$(CC) $(CFLAGS) -c $< 

pow.o: pow.c pow.h
	$(CC) $(CFLAGS) -c $< 
#----------------------------------------------------------------------------------------------------------------------------
run_all1: ./monitor  ./monitor  ./minero 200 100  #Es posible que esto haga que se ejecuten en orden, no simultaneamente

run_mins3: 
	./minero 12 4 & ./minero 18 5 & ./minero 15 8 &
run_minsp1:
	./minero 4 4 & ./minero 6 16 & ./minero 8 8 &

run_mins10:
	./minero 10 40 & ./minero 14 50 & ./minero 15 80 & ./minero 6 4 & ./minero 4 5 & ./minero 15 8 & ./minero 12 30 & ./minero 2 5 & ./minero 6 3 & ./minero 9 10 &

run_mon:
	./monitor

run_monv:
	valgrind --leak-check=full --show-leak-kinds=all ./monitor

run_min:
	./minero 10 5

run_minv3:
	valgrind --leak-check=full --show-leak-kinds=all ./minero 12 10 & valgrind --leak-check=full --show-leak-kinds=all ./minero 9 7  & valgrind --leak-check=full --show-leak-kinds=all ./minero 7 10 &
#----------------------------------------------------------------------------------------------------------------------------
clean:
	rm -v monitor minero *.o
clean_file:
	rm reg*

clean_mon:
	rm -v monitor  $(MON_OBJS)

clean_min:
	rm -v  minero $(MIN_OBJS)
cs:
	ls /dev/shm
	ls /dev/mqueue

rs:
	 rm /dev/shm/shm*
	 rm /dev/shm/sem*
	 rm /dev/mqueue/mqueue
	
kill:
	ps -e | grep "minero" | cut -c-7 | xargs kill -9
kill2:
	ps -e | grep "monitor" | cut -c-7 | xargs kill -9