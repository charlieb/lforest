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
	tree->next_score = 0;
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
  
  int last_score, i;


  for(i=0; i < N_TREES; ++i) {
    if(trees[i].score >= trees[i].next_score) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);
      trees[i].next_score = 1 + trees[i].next_score * 2;
    }
    else
      trees[i].score--;
	
	
		last_score = trees[i].score;
		score_tree(&trees[i]);
	
	/*tree_lights(struct tree trees[N_TREES]);*/
	/*    printf("tree[%i].score = %i\n", i, trees[i].score); */
	
	/* if your score is zero, you DIE */
	/* If a tree dies it gets randomly re-initialized */
	if(trees[i].score <= 0) {
		if(n_dead_trees < N_TREES)
			dead_trees[n_dead_trees++] = i;
		reset_tree(&trees[i]);
		init_sapling(&trees[i]);
		gen_branches(&trees[i]);
	}
  }
  /*printf("%i trees died\n", n_dead_trees); */
}

void iterate_single_light_forest(struct tree trees[N_TREES])
{
	struct ray *rays = 0;
	int nrays = 100, i, j, r;
  float ray_angle_inc, ray_angle = 0.0;
	struct line vec;
	struct point pos, itsec;
	float closest = 100000000, distance;
	int closest_leaf, closest_tree;
	
	rays = malloc(nrays * sizeof(struct ray));
	if(NULL == rays) {
		printf("iterate_single_light_forest: failed to allocate rays\n");
		exit(1);
	}
	
	ray_angle_inc = 360 / (float)nrays;
	
  /* regular ray distribution */
  for(i=0; i < nrays; ++i) {
    vec.start.x = pos.x + 5 - 10 * (float)rand() / (float)RAND_MAX;
    vec.start.y = pos.y + 5 - 10 * (float)rand() / (float)RAND_MAX;
    ray_angle += ray_angle_inc;
    vec.end.x = sin_cache((int)ray_angle);
    vec.end.y = cos_cache((int)ray_angle);
    get_equation(&vec, &rays[i].ray_eq);
    rays[i].direction = vec.end;   
    /*
			printf("Ray: %fx + %f: (%f, %f)\n", 
			tree->rays[i].ray_eq.m, tree->rays[i].ray_eq.c,
			tree->rays[i].direction.x, tree->rays[i].direction.y);
    */
  }

	for(i=0; i < N_TREES; ++i) {
    if(trees[i].score >= trees[i].next_score) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);
      trees[i].next_score = 1 + trees[i].next_score * 2;
    }
    else
      trees[i].score--;
	}

	for(r = 0; r < nrays; ++r) {
		for(i=0; i < N_TREES; ++i)
			for(j = 0; j < trees[i].n_leaves; ++j)
				if(leaf_ray_intersect(&rays[r], 
															trees[i].branches + trees[i].leaves[j], 
															&itsec)) {
					distance = dist(&trees[i].pos, &itsec);
					if(distance < closest) {
						closest = distance;
						closest_tree = i;
						closest_leaf = j;
					}
				}
		trees[closest_tree].score++;
	}
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
