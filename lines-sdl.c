#include "lines-sdl.h"

SDL_Surface *make_sdl_surface(int width, int height) 
{
  SDL_Surface *screen = NULL;

  /*
  printf("Initialising SDL surface %i x %i\n", width, height);
  fflush(NULL);
  */

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

  screen = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE | SDL_DOUBLEBUF);
  if ( screen == NULL ) {
    fprintf(stderr, "Couldn't set %dx%d video mode: %s\n",
	    width, height, SDL_GetError());
    exit(1);
  }
  return screen;
}

void draw_tree(struct tree *tree, SDL_Surface **surface)
{
  int go = 1, i, leaf;
  unsigned long colour;
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


  leaf = 0;
  for (i=0; i < tree->n_branches; ++i) {
    if(tree->leaves[leaf] == i) {
      colour = 0x00FF00FF;
      if(leaf < tree->n_leaves - 1) leaf++;
    }
    else
      colour = 0xAAAA00FF;
    
    lineColor(*surface, 
	      tree->pos.x - tree->branches[i].start.x, 
	      tree->pos.y - tree->branches[i].start.y,
	      tree->pos.x - tree->branches[i].end.x,
	      tree->pos.y - tree->branches[i].end.y,
	      colour);
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

int draw_trees(struct tree trees[], int n_trees, SDL_Surface **surface)
{
  int go = 1, i, tree, leaf;
  static int cont = 1;
  unsigned long colour;
  SDL_Event event;

  if(NULL == *surface)
    *surface = make_sdl_surface(640, 480);
  else
    SDL_FillRect(*surface, NULL, 0x000000);
  
  for(tree=0; tree < n_trees; ++tree) {
    leaf = 0;
    for (i=0; i < trees[tree].n_branches; ++i) {
      colour = 0xAAAA00FF;
      /* If branch is a leaf colour it green */
      if(leaf < trees[tree].n_leaves)
				if(trees[tree].leaves[leaf] == i) {
					colour = 0x00FF00FF;
					leaf++;
				}
			
      lineColor(*surface, 
								trees[tree].pos.x - trees[tree].branches[i].start.x, 
								trees[tree].pos.y - trees[tree].branches[i].start.y,
								trees[tree].pos.x - trees[tree].branches[i].end.x,
								trees[tree].pos.y - trees[tree].branches[i].end.y,
								colour);
    }
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
					return -1;
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
	
  return 0;

  /*  SDL_FreeSurface(*surface);
  *surface = NULL;
  */
}
