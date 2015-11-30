COMPILE=gcc
CFLAGS=-Wall
SOURCE=crack.c
EXECUTABLE=crack

all: clean crack

clean: 
	rm -rf *.bin *.sha1 ${EXECUTABLE}

crack:
	${COMPILE} $(CFLAGS) $(SOURCE) -o ${EXECUTABLE}
