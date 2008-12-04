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

  screen = SDL_SetVideoMode(width, height, 24, SDL_SWSURFACE | SDL_DOUBLEBUF);
  if ( screen == NULL ) {
    fprintf(stderr, "Couldn't set %dx%d video mode: %s\n",
	    width, height, SDL_GetError());
    exit(1);
  }
  return screen;
}

void save_frame(SDL_Surface *surface)
{
	static int frame_no = 0;

	char *home = getenv("HOME");
	char *filename = NULL;

	filename = malloc(strlen(home) + 15);
	sprintf(filename, "%s/forest%.3i.jpg", home, ++frame_no);

	unsigned char *image = malloc(surface->w * surface->h * 3);
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = NULL;

	for(int x = 0; x < surface->w; ++x)
		for(int y = 0; y < surface->h; ++y) {
			/* Here p is the address to the pixel we want to set */
			p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
			image[(x + y * surface->w) * 3 + 0] = 
				(SDL_BYTEORDER == SDL_BIG_ENDIAN ? p[0] : p[2]);
			image[(x + y * surface->w) * 3 + 1] = p[1];
			image[(x + y * surface->w) * 3 + 2] = 				
				(SDL_BYTEORDER == SDL_BIG_ENDIAN ? p[2] : p[0]);
		}
	write_jpeg(image, surface->w, surface->h, filename);
	printf("\nSaved image: %s\n", filename);
	free(image);
}

void draw_tree(struct tree *tree, SDL_Surface **surface)
{
  int go = 1, i, leaf;
  unsigned long colour;
  static int cont = 1;
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
					break;
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

int draw_forest(struct forest *forest, SDL_Surface **surface)
{
  int go = 1, i, t, leaf;
  static int cont = 1;
  unsigned long colour;
  SDL_Event event;

  if(NULL == *surface)
    *surface = make_sdl_surface(640, 480);
  else
    SDL_FillRect(*surface, NULL, 0x000000);


	for(i = 0; i < forest->config.nrays; ++i) {
		if((float)rand() / (float)RAND_MAX > 0.25) continue;
		pthread_mutex_lock(&forest->ray_lines_mtx);
		lineColor(*surface, 
							forest->ray_lines[i].start.x, forest->ray_lines[i].start.y,
							forest->ray_lines[i].end.x, forest->ray_lines[i].end.y,
							0xFFFFFF20);
		pthread_mutex_unlock(&forest->ray_lines_mtx);
	}
  
  for(t = 0; t < forest->config.ntrees; ++t) {
    leaf = 0;
		pthread_mutex_lock(&forest->trees_mtx);
    for (i = 0; i < forest->trees[t].n_branches; ++i) {
      colour = 0xAAAA00FF;
      /* If branch is a leaf colour it green */
      if(leaf < forest->trees[t].n_leaves)
				if(forest->trees[t].leaves[leaf] == i) {
					colour = 0x00FF00FF;
					leaf++;
				}
			
      lineColor(*surface, 
								forest->trees[t].pos.x + forest->trees[t].branches[i].start.x, 
								forest->trees[t].pos.y + forest->trees[t].branches[i].start.y,
								forest->trees[t].pos.x + forest->trees[t].branches[i].end.x,
								forest->trees[t].pos.y + forest->trees[t].branches[i].end.y,
								colour);
    }
		pthread_mutex_unlock(&forest->trees_mtx);
  }

  SDL_Flip(*surface);
  
  while(go) {
    while(SDL_PollEvent(&event)) 
      switch(event.type) {
      case SDL_QUIT: 
				return -1; 
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
					printf((cont < 0 ? "Paused\n" : "Unpaused\n"));
					break;
				case SDLK_s:
					save_frame(*surface);
					break;
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

void *draw_forest_thread_start(void *forest_str)
{
	struct forest *forest = (struct forest*)forest_str;
  SDL_Surface *screen = NULL; 

	while(!draw_forest(forest, &screen)) {
		printf("^");
		usleep(500000);
	}

  SDL_FreeSurface(screen);

	forest->stop = 1;

	return NULL;
}
