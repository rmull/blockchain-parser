CC = gcc
CFLAGS = -Wall -g
TARGET = parse

all: $(TARGET)

$(TARGET): main.o parse.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o parse.o

main.o:
	$(CC) -c main.c 

parse.o:
	$(CC) -c parse.c

clean:
	rm -f *.o parser
