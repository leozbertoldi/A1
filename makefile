#Makefile para o vinac
#Autoria Leonardo ZB 

CC = gcc
CFLAGS= -Wall -Wextra -g -std=c99 

all: main.o func.o
	$(CC) -o vinac main.o func.o

func.o: func.c func.h
	$(CC) -c $(CFLAGS) func.c

main.o: main.c func.h
	$(CC) -c $(CFLAGS) main.c

clean:
	rm -f *.o *~ vinac

