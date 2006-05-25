CC=gcc
flags=-pedantic -Wall -g
sdl_incl=`sdl-config --cflags`
sdl_lib=`sdl-config --libs` -lSDL_gfx
libs=-lm
src=.
dest=bin


l-sys: tree genetics turtle lines-sdl geometry symbols
	$(CC) $(flags) $(includes) \
	-o $(dest)/l-sys \
	$(dest)/turtle.o $(dest)/lines-sdl.o $(dest)/symbols.o \
	$(dest)/tree.o $(dest)/geometry.o $(dest)/genetics.o \
	$(src)/main.c \
	$(libs) $(sdl_incl) $(sdl_lib)

genetics:
	$(CC) $(flags) $(includes) \
	-o $(dest)/genetics.o \
	-c $(src)/genetics.c

tree:
	$(CC) $(flags) $(includes) \
	-o $(dest)/tree.o \
	-c $(src)/tree.c

turtle:
	$(CC) $(flags) $(includes) \
	-o $(dest)/turtle.o \
	-c $(src)/turtle.c

geometry:
	$(CC) $(flags) $(includes) \
	-o $(dest)/geometry.o \
	-c $(src)/geometry.c

symbols:
	$(CC) $(flags) $(includes) \
	-o $(dest)/symbols.o \
	-c $(src)/symbols.c

lines-sdl:
	$(CC) $(flags) $(includes) $(sdl_incl) \
	-o $(dest)/lines-sdl.o \
	-c $(src)/lines-sdl.c
