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

void grow_sapling(struct tree* tree, int iterations)
{
 int init;
 
 for(init=0; init < iterations; ++init)
   expand_rule(tree->expansion, 
	       &tree->exp_size, &tree->seed);
 tree->iterations = iterations;

 tree->pos.x = (int)(WIDTH * (float)random() / RAND_MAX);
 tree->pos.y = (int)(HEIGHT * (float)random() / RAND_MAX);
}

void init_sapling(struct tree* tree)
{
  tree->score = 5;
	tree->next_score = 10;
  random_rule_set(&tree->seed);
  grow_sapling(tree, 1);
}

void init_forest(struct tree trees[N_TREES])
{
  int i;

  srandom((unsigned)time(NULL));
  
  for(i=0; i < N_TREES; ++i) {
		init_tree(&trees[i]);
    init_sapling(&trees[i]);
	}
}

int closest_hit(struct tree trees[N_TREES], struct ray *ray)
{
	float closest, distance;
	int closest_tree = -1, leaf_no, t, b;
	struct line abs_leaf;
	struct point itsec;

	closest = FLT_MAX;
	for(t = 0; t < N_TREES; ++t)
		//for(b = 0; b < trees[t].n_branches; ++b) {
		for(b = 0; b < trees[t].n_leaves; ++b) {
			leaf_no = trees[t].leaves[b];
			absolute_branch(&trees[t], leaf_no, &abs_leaf);
			if(leaf_ray_intersect(&abs_leaf, ray, &itsec)) {
				distance = dist(&ray->origin, &itsec);
				if(distance < closest) {
					closest = distance;
					closest_tree = t;
				}
			}
		}
	return closest_tree;
}

void light_trees(struct tree trees[N_TREES], int nrays)
{
	struct ray ray;
	float ray_angle;
	int closest_tree = 0;
	int r;
	
	for(r = 0; r < nrays; ++r) {
		ray.origin.x = WIDTH * (float)rand() / (float)RAND_MAX;
		ray.origin.y = HEIGHT * (float)rand() / (float)RAND_MAX;
		//ray_angle = 0.0;
		ray_angle = 360 * (float)rand() / RAND_MAX;
		
		ray.direction.x = sin_cache((int)ray_angle);
		ray.direction.y = cos_cache((int)ray_angle);
		closest_tree = closest_hit(trees, &ray);
					
		if(-1 != closest_tree) {
			trees[closest_tree].score++;
		}
	}
}

void iterate_forest(struct tree trees[N_TREES])
{
	const int nrays = 10000;
	int i;

	/* if your score is zero, you DIE */
	/* If a tree dies it gets randomly re-initialized */
	for(i = 0; i < N_TREES; ++i) {
		//printf("trees[%i].score = %i\n", i, trees[i].score);
		if(trees[i].score <= -10) {
			/*if(n_dead_trees < N_TREES)
				dead_trees[n_dead_trees++] = i;*/
			//printf("trees[%i] died\n", i);
			reset_tree(&trees[i]);
			init_sapling(&trees[i]);
			gen_branches(&trees[i]);
		}
	}
	
	light_trees(trees, nrays);

	for(i = 0; i < N_TREES; ++i) {
		trees[i].score -= 
			MAX(1, 
					trees[i].n_leaves * 0.5 + 
					(trees[i].n_branches - trees[i].n_leaves) * 1.00);

    if(trees[i].score >= trees[i].next_score) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);
      trees[i].next_score = 1 + trees[i].next_score * 2;
    }
	}
}

void breed_forest(struct tree trees[N_TREES])
{
  int replace, parent1, parent2, child;
	float weights[N_TREES], inverted_weights[N_TREES];

  generate_weights(trees, N_TREES, weights);
	invert_weights(weights, N_TREES, inverted_weights);

	//  for(replace = 0; replace < (int)(N_TREES * 0.125); ++replace) {
	for(replace = 0; replace < N_TREES; ++replace) {

		child = roulette_select(inverted_weights, N_TREES);

		if(trees[child].score > 0)
			continue;
		else if(random() / (float)RAND_MAX > 0.25) {
			/* some dead trees randomly re-initialized to introduce "fresh blood" */
			reset_tree(&trees[child]);
			init_sapling(&trees[child]);
			gen_branches(&trees[child]);
			continue;
		}
		/*
		else
			child = replace;
		*/
    /* Select the parents and a place for the child */
		//child = roulette_select(inverted_weights, N_TREES);
    
    parent1 = child;
    while(child == parent1)
      parent1 = roulette_select(weights, N_TREES);
    
		parent2 = child;
    while(child == parent2)
      parent2 = roulette_select(weights, N_TREES);
		
    /*parent2 = pick_neighbour(parent1, trees);*/
    /*child = pick_neighbour(parent1, trees);*/
		/*
    printf("Breeding: %i(%i) + %i(%i) = %i(%i)\n", 
	   parent1, trees[parent1].score,
	   parent2, trees[parent2].score,
	   child,   trees[child].score);
		*/
    crossover(&trees[parent1], &trees[parent2], &trees[child]);

		reset_tree(&trees[child]);
    grow_sapling(&trees[child], 1);
    gen_branches(&trees[child]);
  }
}

int draw_forest(struct tree trees[N_TREES], SDL_Surface **screen)
{
  return draw_trees(trees, N_TREES, screen);
}

void write_forest(FILE *file, struct tree trees[N_TREES])
{
	int i = N_TREES;
	fwrite((void*)&i, sizeof(int), 1, file);
	for(i = 0; i < N_TREES; ++i)
		write_tree(file, &trees[i]);
}

void read_forest(FILE *file, struct tree trees[N_TREES])
{
	int i = N_TREES;
	/* atm N_TREES is a #define constant :( */
	fread((void*)&i, sizeof(int), 1, file);
	for(i = 0; i < N_TREES; ++i)
		read_tree(file, &trees[i]);
}
