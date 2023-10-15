# Add cross compilation support
CC=${CROSS_COMPILE}

# Set flags
CFLAGS = -g -Wall -Werror

all: aesdsocket
	@echo "build done!"

aesdsocket: aesdsocket.o
	$(CC)gcc $(CFLAGS) aesdsocket.o -o aesdsocket

aesdsocket.o: aesdsocket.c
	$(CC)gcc $(CFLAGS) -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f *.o aesdsocket
