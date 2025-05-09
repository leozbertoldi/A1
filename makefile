#Makefile para o vinac
#Autoria Leonardo ZB 

CC = gcc
CFLAGS= -Wall -Wextra -g 

all: main.o func.o lz.o
	$(CC) -o vinac main.o func.o lz.o

func.o: func.c func.h
	$(CC) -c $(CFLAGS) func.c

lz.o: lz.c lz.h
	$(CC) -c $(CFLAGS) lz.c

main.o: main.c func.h
	$(CC) -c $(CFLAGS) main.c

clean:
	rm -f *.o *~ vinac

