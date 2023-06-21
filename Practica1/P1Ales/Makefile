CC=gcc 
CFLAGS = -ansi -pedantic -g -Wall -ggdb 
#CFLAGS = -Wall -g -DNDEBUG #-O3 				#-DNDEBUG turns of asserts mandated by the C standards


IFLAGS=-I. ##La opción -I se usa para indicar donde se encuentran los ficheros cabeceras (.h)
LDFLAGS=-L. ##La opción -L indica el directorio donde se encuentra las biblioteca

LIBS= -lm -pthread ### -lm enlaza la biblioteca matematica, -pthread enlaza la biblioteca de hilos

## EXECUTIVES CREATION ##
#----------------------------------------------------------------------------------------------------------------------------


mrush: mrush.c pow.c
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

mrush152: mrush152.c pow.c
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

mrushfork: mrushfork.c pow.c
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

#----------------------------------------------------------------------------------------------------------------------------