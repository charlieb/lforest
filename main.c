#include <stdio.h>
#include <stdlib.h>

#include "turtle.h"
#include "types.h"
#include "lines-sdl.h"
#include "symbols.h"
#include "tree.h"

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
  score_tree(&tree);
 
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
  score_tree(&tree);
 
}

int uniform_select(int n)
{
  return (int)(n * (random() / (float)RAND_MAX));
}

int roulette_select(float *weights, int n_weights)
{
  float rnd = random() / (float)RAND_MAX;
  float total = weights[0];
  int w = 0;

  while(total < rnd) total += weights[++w];
  return w;
}

void generate_weights(struct tree *trees, int n_trees, float *weights)
{
  int i;
  float total = 0.0;
  for(i=0; i < n_trees; ++i) total += trees[i].score;
  for(i=0; i < n_trees; ++i) {
    weights[i] = (float)trees[i].score / total;
    /*    printf("Weight[%i] = %f\n", i, weights[i]); */
  }
}


void test()
{
  struct tree trees[100];
  int n_trees = 100;

  float weights[100];
  int i, j, replace, parent1, parent2, child;
  int w, h, wc, hc;
  SDL_Surface *screen = NULL;

  for(i=0; i < n_trees; ++i) {
    trees[i].seed.rule_size = 10;
    trees[i].seed.num_rules = 2;
    init_tree(&trees[i]);
    trees[i].pos.x = 320;
    trees[i].pos.y = 240;
    trees[i].score = 10;
    randomize_tree(&trees[i]);
  }

  for(j=0; j < 1000; ++j) {
    for(i=0; i < n_trees; ++i) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);

      trees[i].score--; 

      score_tree(&trees[i]);

      /*
	print_syms(trees[i].expansion,trees[i].exp_size,
	trees[i].seed.num_rules);
 	printf("--------\n");
	print_rule_set(&trees[i].seed);
	fflush(NULL);
      */
      
      printf("%i: tree %i, its %i scored %i\n", j , i, 
	     trees[i].iterations,
	     trees[i].score);
      
      /* if your score is zero, you DIE */
      if(trees[i].score <= 0) {
	printf("Tree Killed\n");
	init_tree(&trees[i]);
	randomize_tree(&trees[i]);
	trees[i].score = 1;
      }
    }

    w = 60;
    h = 40;
    for(wc=0; wc < 10; ++wc)
      for(hc=0; hc < 10; ++hc) {
	trees[wc * 10 + hc].pos.x = 20 + wc * w;
	trees[wc * 10 + hc].pos.y = 40 + hc * h;
      }

    draw_trees(trees, n_trees, &screen);
    
    generate_weights(trees, n_trees, weights);
    for(replace=0; replace < 25; ++replace) {
      /* Select the parents and a place for the child */
      parent1 = roulette_select(weights, n_trees);
      parent2 = roulette_select(weights, n_trees);
      /* Child cannot be either of the parents */
      child = parent1;
      while(child == parent1 || child == parent2)
	child = uniform_select(n_trees);
      /*
      printf("Breeding: %i + %i -> %i\n", parent1, parent2, child);
      */
      /* Generate the child */
      /*      print_rule_set(&trees[parent1].seed); 
      printf("+\n");
      print_rule_set(&trees[parent2].seed); 
      printf("->\n");
      */

      /* Charge the parents for having kids */
      
      crossover(&trees[parent1], &trees[parent2], &trees[child]);

      /*
      trees[parent1].score -= 5;
      trees[parent2].score -= 5;
      */
      /*
      print_rule_set(&trees[child].seed);  
      printf("-------\n");
      */
    }

    SDL_FreeSurface(screen);
  }
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

