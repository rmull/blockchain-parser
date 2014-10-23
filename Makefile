CFLAGS = -Wall -g
SRC = main.c parse.c
OBJ = ${SRC:.c=.o}
TARGET = parse

all: ${TARGET}

.c.o:
	@${CC} -c ${CFLAGS} $<

${TARGET}: ${OBJ}
	@${CC} -o $@ ${OBJ}

clean:
	@rm -f ${TARGET} ${OBJ}
