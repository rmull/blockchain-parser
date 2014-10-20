all: parse

parse: main.o parse.o
	$(CC) main.o parse.o -o parser

main.o:
	$(CC) -c main.c 

parse.o:
	$(CC) -c parse.c

clean:
	rm -f *.o parser