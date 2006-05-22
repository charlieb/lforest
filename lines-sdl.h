#ifndef LINES_SDL_H
#define LINES_SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include "turtle.h"
#include "types.h"

SDL_Surface *make_sdl_surface(int width, int height);
void draw_tree(struct tree *tree, SDL_Surface **surface);
void draw_trees(struct tree trees[], int n_trees, SDL_Surface **surface);
#endif
