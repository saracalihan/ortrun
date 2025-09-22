CFLAGS=-Wall -Wextra -ggdb
CC=cc

compile:
	cc ${CFLAGS} -o build/ortrun ortrun.c