#ifndef FOREST_H
#define FOREST_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "types.h"
#include "tree.h"
#include "turtle.h"
#include "genetics.h"
#include "lines-sdl.h"
#include "kd-tree.h"

void alloc_forest(struct forest *forest);
void init_forest(struct forest *forest);
void iterate_forest(struct forest *forest);
void breed_forest(struct forest *forest);
int draw_forest(struct forest *forest, SDL_Surface **screen);

void write_forest(FILE *file, struct forest *forest);
void read_forest(FILE *file, struct forest *forest);
void free_forest(struct forest *forest);
#endif
