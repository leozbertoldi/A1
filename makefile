#Makefile para o vinac
#Autoria Leonardo ZB 

CC = gcc
CFLAGS= -Wall -Wextra -g 
TARGET_DIR = login
TARGET = $(TARGET_DIR)/vinac

all: $(TARGET)

$(TARGET): main.o func.o lz.o | $(TARGET_DIR)
	$(CC) -o $@ main.o func.o lz.o

func.o: func.c func.h
	$(CC) -c $(CFLAGS) func.c

lz.o: lz.c lz.h
	$(CC) -c $(CFLAGS) lz.c

main.o: main.c func.h
	$(CC) -c $(CFLAGS) main.c

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

clean:
	rm -f *.o *~ vinac
	rm -rf $(TARGET_DIR)

