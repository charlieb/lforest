#ifndef FOREST_H
#define FOREST_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "tree.h"
#include "turtle.h"
#include "genetics.h"
#include "lines-sdl.h"

#define WIDTH 32
#define HEIGHT 24
#define N_TREES WIDTH * HEIGHT
#define TREE_SPACE 20

void index2coords(int index, int *x, int *y);
int coords2index(int x, int y);
int pick_neighbour(int parent, struct tree trees[N_TREES]);
void init_sapling(struct tree* tree);
void init_forest(struct tree trees[N_TREES]);
void iterate_forest(struct tree trees[N_TREES]);
void breed_forest(struct tree trees[N_TREES]);
int draw_forest(struct tree trees[N_TREES], SDL_Surface *screen);

#endif
