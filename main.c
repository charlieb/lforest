#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "turtle.h"
#include "types.h"
#include "lines-sdl.h"
#include "symbols.h"
#include "tree.h"
#include "forest.h"
#include "genetics.h"
#include "kd-tree.h"

void test_tree(struct tree *tree, int its)
{
  int i;
  char *rule;

  init_tree(tree);

  tree->pos.x = 0;
  tree->pos.y = 0;

  tree->seed.rule_size = 10;
  tree->seed.num_rules = 1;
  init_rule_set(&tree->seed);

  rule = get_rule(0, &tree->seed);
  strncpy(rule, "(++!0)--!0", 10);  
  chars_to_rule(rule, tree->seed.rule_size, tree->seed.num_rules);

  /* Expand the tree N times*/
  for(i=0; i < its; ++i) { 
    expand_rule(tree->expansion, &tree->exp_size, &tree->seed);
    /*print_syms(tree->expansion, tree->exp_size, tree->seed.num_rules);*/
  }

  gen_branches(tree);
  printf("Generated Branches\n");
  tree->pos.x = 320;
  tree->pos.y = 400;
}

void perfect_tree(struct tree *tree)
{
   /*  SDL_Surface *screen; */

  init_tree(tree);
	tree->pos.x = 100;
	tree->pos.y = 100;
  tree->iterations = 17;
  tree->exp_size = 34 * 4;
  strncpy(tree->expansion,
					"(!(+++++++++!)---------!)+++++++++"
					"(!(+++++++++!)---------!)+++++++++"
					"(!(+++++++++!)---------!)+++++++++"
					"(!(+++++++++!)---------!)+++++++++",
					tree->exp_size);
  
  gen_branches(tree);
  /*  print_leaves(&tree); */
}

void half_tree(int its)
{
  struct tree tree;
  /*  SDL_Surface *screen; */

  init_tree(&tree);
  tree.iterations = its;
  tree.exp_size = 34 * 4;
  strncpy(tree.expansion,
	  "(!(+++++++++!)----------)+++++++++"
	  "(!(+++++++++!)----------)+++++++++"
	  "(!(+++++++++!)----------)+++++++++"
	  "(!(+++++++++!)----------)+++++++++",
	  tree.exp_size);
  
  gen_branches(&tree);
  /* print_leaves(&tree); */
}

void test()
{
  struct forest forest;

  int i;

	FILE *forest_file = NULL;
	char *home = getenv("HOME");
	char *forest_filename = NULL;

	pthread_t draw_thread;

	forest_filename = malloc(strlen(home) + 10);
	sprintf(forest_filename, "%s/.lforest", home);

	forest.config.nrays = 10000;
	forest.config.init_score = 5;
	forest.config.init_iterations = 1;
	forest.config.replace_trees = 0.125;
	forest.config.leaf_cost = 0.5;
	forest.config.branch_cost = 1.0;
	forest.config.re_init_chance = 0.25;
	forest.config.width = 640;
	forest.config.height = 480;

	forest_file = fopen(forest_filename, "rb");
	if(NULL == forest_file) {
		forest.config.ntrees = 250;
		init_forest(&forest);
		printf("%s not found: using random trees\n", forest_filename);
		fflush(NULL);
	}
	else {
		printf("%s found: using saved trees\n", forest_filename);
		fflush(NULL);
		read_forest(forest_file, &forest);
		fclose(forest_file);
		forest_file = NULL;
	}

	/* Launch the display thread */
	forest.stop = 0;
	pthread_create(&draw_thread, NULL, draw_forest_thread_start, (void*)&forest);

	for(i = 0; 0 == forest.stop; ++i) {
    iterate_forest(&forest);
    breed_forest(&forest);
		printf("."); fflush(NULL);
  }

	pthread_join(draw_thread, NULL);
	
  printf("%i iterations\n", i);
  
	forest_file = fopen(forest_filename, "wb");
	
	if(NULL == forest_file) 
		printf("Failed to open %s: not saving\n", forest_filename);
	else {
		printf("saving to %s\n", forest_filename);
		write_forest(forest_file, &forest);
		fclose(forest_file);
	}

	free_forest(&forest);
	free(forest_filename);
}


int main(int argc, char **argv)
{
  /*  
  int i;
  for(i=0; i < 100; ++i) half_tree(i); 
  */
  /*
  test_tree();
  */
	//test_kd_tree();
	srand(time(NULL));
  test();

  printf("Normal Exit\n");
  return 0;
}

