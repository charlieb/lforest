#ifndef LINES_SDL_H
#define LINES_SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include "turtle.h"
#include "types.h"
#include "jpeg.h"

SDL_Surface *make_sdl_surface(int width, int height);
void draw_tree(struct tree *tree, SDL_Surface **surface);
int draw_forest(struct forest *forest, SDL_Surface **surface);
void *draw_forest_thread_start(void *forest_str);
#endif
