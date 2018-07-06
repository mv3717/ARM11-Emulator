CC = gcc
CFLAGS = -Wall -g -pedantic -std=c11 -D_BSD_SOURCE
BUILD = main 



main: main.o assemble.o utils.o parser.o symboltable.o
		$(CC) main.o assemble.o utils.o parser.o symboltable.o -o ../output/assemble

main.o: main.c typedefs.h utils.h assemble.h parser.h symboltable.h
		$(CC) $(CFLAGS) -c main.c

assemble.o: assemble.c assemble.h typedefs.h symboltable.h
		$(CC) $(CFLAGS) -c assemble.c

parser.o: parser.c parser.h typedefs.h
		$(CC) $(CFLAGS) -c parser.c

utils.o: utils.c utils.h typedefs.h 
		$(CC) $(CFLAGS) -c utils.c 

symboltable.o: symboltable.c symboltable.h typedefs.h
	$(CC) $(CFLAGS) -c symboltable.c

clean:
		rm -f *.o main


