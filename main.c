#include <stdio.h>
#include <stdlib.h>

#include "turtle.h"
#include "types.h"
#include "lines-sdl.h"
#include "symbols.h"
#include "tree.h"
#include "forest.h"
#include "genetics.h"
#include "kd-tree.h"

void test_tree()
{
  int i;
  struct tree tree;
  char *rule;

  SDL_Surface *screen;

  init_tree(&tree);

  tree.pos.x = 0;
  tree.pos.y = 0;

  tree.seed.rule_size = 10;
  tree.seed.num_rules = 1;
  init_rule_set(&tree.seed);

  rule = get_rule(0, &tree.seed);
  strncpy(rule, "(++!0)--!0", 10);  
  chars_to_rule(rule, tree.seed.rule_size, tree.seed.num_rules);

  /* Expand the tree N times*/
  for(i=0; i < 8; ++i) { 
    expand_rule(tree.expansion, &tree.exp_size, &tree.seed);
    /*print_syms(tree.expansion, tree.exp_size, tree.seed.num_rules);*/
  }

  gen_branches(&tree);
  printf("Generated Branches\n");
  tree.pos.x = 320;
  tree.pos.y = 400;
  draw_tree(&tree, &screen); 

  SDL_FreeSurface(screen);
  /*  free_tree(&tree); */
}

void perfect_tree()
{
  struct tree tree;
  /*  SDL_Surface *screen; */

  init_tree(&tree);
  tree.iterations = 17;
  tree.exp_size = 34 * 4;
  strncpy(tree.expansion,
	  "(!(+++++++++!)---------!)+++++++++"
	  "(!(+++++++++!)---------!)+++++++++"
	  "(!(+++++++++!)---------!)+++++++++"
	  "(!(+++++++++!)---------!)+++++++++",
	  tree.exp_size);
  
  gen_branches(&tree);
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
  struct tree trees[N_TREES];

  int i, draw_ret = 0;

  SDL_Surface *screen = NULL; 

	FILE *forest_file = NULL;
	char *home = getenv("HOME");
	char *forest_filename = NULL;

	forest_filename = malloc(strlen(home) + 10);
	sprintf(forest_filename, "%s/.lforest", home);

	memset(trees, 0, N_TREES * sizeof(struct tree));
  init_forest(trees);

	forest_file = fopen(forest_filename, "rb");
	if(NULL == forest_file) {
		printf("%s not found: using random trees\n", forest_filename);
		fflush(NULL);
	}
	else {
		printf("%s found: using saved trees\n", forest_filename);
		fflush(NULL);
		read_forest(forest_file, trees);
		fclose(forest_file);
		forest_file = NULL;
	}

	/*****/
	struct node *nodes;
	int nnodes;
	nodes_from_trees(trees, N_TREES, &nodes, &nnodes);

	struct node **xnodes = malloc(nnodes * sizeof(struct node*));
	struct node **ynodes = malloc(nnodes * sizeof(struct node*));
	
	sort_nodes(xnodes, ynodes, nodes, nnodes);
	printf("Sorted %i branches, twice!\n", nnodes);
	fflush(NULL);

	struct kd_node kd_head;

	build_kd_tree(xnodes, ynodes, nnodes, 1, &kd_head);

	/*****/

  for(i = 0; draw_ret == 0; ++i) {
    iterate_forest(trees);
    draw_ret = draw_forest(trees, &screen);
    breed_forest(trees);
  }

  printf("%i iterations\n", i);
  
	forest_file = fopen(forest_filename, "wb");
	
	if(NULL == forest_file) 
		printf("Failed to open %s: not saving\n", forest_filename);
	else {
		printf("saving to %s\n", forest_filename);
		write_forest(forest_file, trees);
		fclose(forest_file);
	}

	for(i=0; i < N_TREES; ++i)
    free_tree(&trees[i]);
		
  SDL_FreeSurface(screen);
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
  test();

  printf("Normal Exit\n");
  return 0;
}

