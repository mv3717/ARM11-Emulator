CC      = gcc
CFLAGS  = -Wall -g -std=c99 

main: haiku.o parser.o main.o grammar.o
	$(CC) $(CFLAGS) haiku.o main.o parser.o grammar.o -o main -lwriringPi

main.o: main.c haiku.h parser.h grammar.h
	$(CC) $(CFLAGS) -c main.c

grammar.o: grammar.c grammar.h
	$(CC) $(CFLAGS) -c grammar.c
  
haiku.o: haiku.c haiku.h grammar.h parser.h
	$(CC) $(CFLAGS) -c haiku.c 

parser.o: parser.c parser.h grammar.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f *.o main
