# Add cross compilation support
CC = gcc

# Set flags
CFLAGS ?= -g -Wall -Werror

all: aesdsocket
	@echo "build done!"

aesdsocket: aesdsocket.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) aesdsocket.o -o aesdsocket

aesdsocket.o: aesdsocket.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f *.o aesdsocket

