CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g -D_GNU_SOURCE
INCLUDES=`sdl-config --cflags`
LIBS=-lm `sdl-config --libs` -lSDL_gfx
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
DEST=bin

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

l-sys: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $(DEST)/l-sys

clean:
	- rm -f $(OBJECTS) $(DEST)/l-sys