CC=gcc 
CFLAGS = -ansi -pedantic -g -Wall -ggdb 


DFLAGS = -D ##DEBUG
IFLAGS=-I. ##La opción -I se usa para indicar donde se encuentran los ficheros cabeceras (.h)

LIBS= -lm -lpthread -lrt ### -lm enlaza la biblioteca matematica, -pthread enlaza la biblioteca de hilos

## EXECUTIVES CREATION ##
#----------------------------------------------------------------------------------------------------------------------------

all: vote 

vote: vot_mult.o vot_func.o vot_utils.o
	$(CC) -o $@ $^ $(LIBS)

	
vot_mult.o: vot_mult.c vot_func.h vot_utils.h
	$(CC) -c $< 

vot_func.o: vot_func.c vot_func.h vot_utils.h
	$(CC) -c $< 

vot_utils.o: vot_utils.c vot_utils.h
	$(CC) -c $< 

#----------------------------------------------------------------------------------------------------------------------------
run:
	./vote 10 5

runv:
	valgrind --leak-check=full --show-leak-kinds=all ./vote 2 10
#----------------------------------------------------------------------------------------------------------------------------
clean:
	rm -v vote *.o

cs:
	ls /dev/shm

rs:
	 rm /dev/shm/sem*