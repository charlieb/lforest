#include "lines-sdl.h"

SDL_Surface *make_sdl_surface(int width, int height) 
{
  SDL_Surface *screen = NULL;

  printf("Initialising SDL surface %i x %i\n", width, height);
  fflush(NULL);
  
  /* Initialize the SDL library */
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    fflush(NULL);
    exit(1);
  }
  /* Clean up on exit */
  atexit(SDL_Quit);
  
  /*
   * Initialize the display in a 640x480 8-bit palettized mode,
   * requesting a software surface
   */

  screen = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE);
  if ( screen == NULL ) {
    fprintf(stderr, "Couldn't set %dx%d video mode: %s\n",
	    width, height, SDL_GetError());
    exit(1);
  }
  return screen;
}

void draw_tree(struct tree *tree, SDL_Surface **surface)
{
  int go = 1, i;
  static int cont = -1;
  SDL_Event event;
  SDL_Rect size;
  size.x = 0;
  size.y = 0;
  size.w = 640;
  size.h = 480;


  if(!*surface) 
    *surface = make_sdl_surface(size.w, size.h);
  else
    SDL_FillRect(*surface, &size, 0x000000);
  

  /* Draw all the lines*/
  for (i=0; i < tree->n_lines; ++i)
    lineColor(*surface, 
	      tree->pos.x - tree->lines[i].start.x, 
	      tree->pos.y - tree->lines[i].start.y,
	      tree->pos.x - tree->lines[i].end.x,
	      tree->pos.y - tree->lines[i].end.y,
	      0xAAAA00FF); 

  /* Redraw only the leaves */
  for(i = 0; i < tree->n_leaves; ++i)
    lineColor(*surface, 
	      tree->pos.x - tree->lines[tree->leaves[i]].start.x, 
	      tree->pos.y - tree->lines[tree->leaves[i]].start.y,
	      tree->pos.x - tree->lines[tree->leaves[i]].end.x,
	      tree->pos.y - tree->lines[tree->leaves[i]].end.y,
	      0x00FF00FF);
  
  SDL_Flip(*surface);
  
  while(go) {
    while(SDL_PollEvent(&event)) 
      switch(event.type) {
      case SDL_QUIT: 
	go = 0; 
	break;
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	case SDLK_q:
	  exit(0);
	case SDLK_RETURN:
	  go = 0;
	  break;
	case SDLK_c:
	  cont = -cont;
	default:
	  break;
	}
      }
    if(cont > 0) go = 0;
  }

  /*  SDL_FreeSurface(*surface);
  *surface = NULL;
  */
}

void draw_trees(struct tree trees[], int n_trees, SDL_Surface **surface)
{
  int go = 1, i, tree;
  static int cont = -1;
  SDL_Event event;
  SDL_Rect size;
  size.x = 0;
  size.y = 0;
  size.w = 640;
  size.h = 480;


  if(!*surface) 
    *surface = make_sdl_surface(size.w, size.h);
  else
    SDL_FillRect(*surface, &size, 0x000000);
  

  for(tree=0; tree < n_trees; ++tree) {
    /* Draw all the lines*/
    for (i=0; i < trees[tree].n_lines; ++i)
      lineColor(*surface, 
		trees[tree].pos.x - trees[tree].lines[i].start.x, 
		trees[tree].pos.y - trees[tree].lines[i].start.y,
		trees[tree].pos.x - trees[tree].lines[i].end.x,
		trees[tree].pos.y - trees[tree].lines[i].end.y,
		0xAAAA00FF); 
    
    /* Redraw only the leaves */
    for(i = 0; i < trees[tree].n_leaves; ++i)
      lineColor(*surface, 
		trees[tree].pos.x - 
		trees[tree].lines[trees[tree].leaves[i]].start.x, 

		trees[tree].pos.y -
		trees[tree].lines[trees[tree].leaves[i]].start.y,

		trees[tree].pos.x -
		trees[tree].lines[trees[tree].leaves[i]].end.x,

		trees[tree].pos.y - 
		trees[tree].lines[trees[tree].leaves[i]].end.y,

		0x00FF00FF);
  }

  SDL_Flip(*surface);
  
  while(go) {
    while(SDL_PollEvent(&event)) 
      switch(event.type) {
      case SDL_QUIT: 
	go = 0; 
	break;
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	case SDLK_q:
	  exit(0);
	case SDLK_RETURN:
	  go = 0;
	  break;
	case SDLK_c:
	  cont = -cont;
	default:
	  break;
	}
      }
    if(cont > 0) go = 0;
  }

  /*  SDL_FreeSurface(*surface);
  *surface = NULL;
  */
}
