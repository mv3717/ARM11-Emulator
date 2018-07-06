CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror


main: main.o executor.o utils.o tests.o data_processing.o
	$(CC) main.o executor.o tests.o utils.o data_processing.o -o ../output/emulate

main.o: main.c executor.c typedefs.h executor.h utils.h data_processing.h
	$(CC) $(CFLAGS) -c main.c

executor.o: executor.c typedefs.h executor.h tests.h utils.h
	$(CC) $(CFLAGS) -c executor.c

tests: tests.o utils.o
	$(CC) tests.o utils.o -o tests

tests.o: tests.c typedefs.h utils.h tests.h
	$(CC) $(CFLAGS) -c tests.c

utils.o: utils.c typedefs.h utils.h
	$(CC) $(CFLAGS) -c utils.c

data_processing.o: data_processing.c utils.h executor.h typedefs.h
	$(CC) $(CFLAGS) -c data_processing.c

clean:
	rm -f *.o main
