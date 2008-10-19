CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -D_GNU_SOURCE
INCLUDES=`sdl-config --cflags`
LIBS=-lm `sdl-config --libs` -lSDL_gfx
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
DEST=bin

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) $(INCLUDES) -fsyntax-only $(CHK_SOURCES)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

l-sys: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $(DEST)/l-sys

debug: CFLAGS += -g 
debug: l-sys

release: CFLAGS += -O2
release: l-sys

profile: CFLAGS += -pg
profile: l-sys

clean:
	- rm -f $(OBJECTS) $(DEST)/l-sys