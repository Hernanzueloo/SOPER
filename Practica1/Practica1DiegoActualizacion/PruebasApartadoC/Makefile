CC=gcc 
CFLAGS = -ansi -pedantic -g -Wall -ggdb 
#CFLAGS = -Wall -g -DNDEBUG #-O3 				#-DNDEBUG turns of asserts mandated by the C standards


IFLAGS=-I. ##La opción -I se usa para indicar donde se encuentran los ficheros cabeceras (.h)
LDFLAGS=-L. ##La opción -L indica el directorio donde se encuentra las biblioteca

LIBS= -lm -pthread ### -lm enlaza la biblioteca matematica, -pthread enlaza la biblioteca de hilos

## EXECUTIVES CREATION ##
#----------------------------------------------------------------------------------------------------------------------------


prueba: prueba_pipe.c 
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

run:
	./prueba


runv:
	valgrind --leak-check=full  --show-leak-kinds=all ./prueba


clean:
	rm -v prueba 
#----------------------------------------------------------------------------------------------------------------------------