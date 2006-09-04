#include "forest.h"

void index2coords(int index, int *x, int *y)
{
  *y = index / WIDTH;
  *x = index - *y * WIDTH;
}

int coords2index(int x, int y)
{
  return y * WIDTH + x;
}

int pick_neighbour(int parent, struct tree trees[N_TREES])
{
  /* Is there an empty slot next to either of the parents? */
  /* If it's in the first row don't check the UP */
  int x, y, xoffset, yoffset;
  index2coords(parent, &x, &y);
  
  xoffset = (int)((float)rand() / (float)RAND_MAX * 4.0 - 2.0);
  while(x + xoffset < 0 || x + xoffset >= WIDTH)
    xoffset = (int)((float)rand() / (float)RAND_MAX * 4.0 - 2.0);
  
  yoffset = (int)((float)rand() / (float)RAND_MAX * 4.0 - 2.0);
  while(y + yoffset < 0 || y + yoffset >= HEIGHT) 
    yoffset = (int)((float)rand() / (float)RAND_MAX * 4.0 - 2.0);

  return coords2index(x + xoffset, y + yoffset);   
}

void grow_sapling(struct tree* tree)
{
 int init;
 
 for(init=0; init < 3; ++init)
   expand_rule(tree->expansion, 
	       &tree->exp_size, &tree->seed);
 tree->iterations = 3;
 tree->score = 1;
}

void init_sapling(struct tree* tree)
{
  tree->seed.rule_size = 7;
  tree->seed.num_rules = 5;
  init_rule_set(&tree->seed);
  init_tree(tree);
  tree->score = 0;
  randomize_tree(tree);
  grow_sapling(tree);
}

void init_forest(struct tree trees[N_TREES])
{
  int i;

  srandom((unsigned)time(NULL));
  
  for(i=0; i < N_TREES; ++i) 
    init_sapling(&trees[i]);
}

void iterate_forest(struct tree trees[N_TREES])
{
  static int dead_trees[N_TREES]; 
  int n_dead_trees = 0; 
  
  static int next_scores[N_TREES];
  int last_score, i;


  for(i=0; i < N_TREES; ++i) {
    if(trees[i].score >= next_scores[i]) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);
      next_scores[i] = 1 + next_scores[i] * 2;
    }
    else
      trees[i].score--;
    
    last_score = trees[i].score;
    score_tree(&trees[i]);

    /*    printf("tree[%i].score = %i\n", i, trees[i].score); */
    
    /* if your score is zero, you DIE */
    /* If a tree dies it gets randomly re-initialized */
          
    if(trees[i].score <= 0) {
      if(n_dead_trees < N_TREES)
	dead_trees[n_dead_trees++] = i;
      reset_tree(&trees[i]);
      init_sapling(&trees[i]);
      gen_branches(&trees[i]);
      next_scores[i] = 0;
    }
  }
  /*printf("%i trees died\n", n_dead_trees); */
}

void breed_forest(struct tree trees[N_TREES])
{
  int replace, parent1, parent2, child;
  static float weights[N_TREES];
  
  generate_weights(trees, N_TREES, weights);
  for(replace=0; replace < (int)(N_TREES * 0.1); ++replace) {
    /* Select the parents and a place for the child */
    child = -1; /*uniform_select(N_TREES);*/
    
    parent1 = child;
    while(child == parent1)
      parent1 = roulette_select(weights, N_TREES);
    
    parent2 = child;
    while(child == parent2)
      parent2 = roulette_select(weights, N_TREES);
    
    parent2 = pick_neighbour(parent1, trees);
    child = pick_neighbour(parent1, trees);

    /*
    printf("Breeding: %i(%i) + %i(%i) = %i(%i)\n", 
	   parent1, trees[parent1].score,
	   parent2, trees[parent2].score,
	   child,   trees[child].score);
    */
    crossover(&trees[parent1], &trees[parent2], &trees[child]);

    grow_sapling(&trees[child]);
    gen_branches(&trees[child]);
  }
}

int draw_forest(struct tree trees[N_TREES], SDL_Surface *screen)
{
  int w, h, wc, hc;

  w = h = TREE_SPACE;
  for(wc=0; wc < WIDTH; ++wc)
    for(hc=0; hc < HEIGHT; ++hc) {
      trees[wc * HEIGHT + hc].pos.x = (float)(TREE_SPACE + wc * w);
      trees[wc * HEIGHT + hc].pos.y = (float)(TREE_SPACE + hc * h);
    }
  return draw_trees(trees, N_TREES, &screen);
}
