CFLAGS=-Wall -Wextra -ggdb
CC=cc

compile:
	cc ${CFLAGS} -o build/ortrun -I da.h ortrun.c