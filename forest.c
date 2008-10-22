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

int pick_neighbour(int parent)
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

void grow_sapling(struct tree* tree, int iterations)
{
 int init;
 
 for(init=0; init < iterations; ++init)
   expand_rule(tree->expansion, 
	       &tree->exp_size, &tree->seed);
 tree->iterations = iterations;
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

void light_trees(struct tree trees[N_TREES], int nlights, int nrays)
{
	struct ray ray;
	struct line abs_leaf;
	struct point itsec;
	float ray_angle, ray_angle_inc =	ray_angle_inc = 360 / (float)nrays;
	float closest, distance;
	int closest_tree = 0, leaf_no;
	char leaf = 0;
	int b, r, t, li;
	
	for(li = 0; li < nlights; ++li) {
		ray.origin.x = 640 * (float)rand() / (float)RAND_MAX;
		ray.origin.y = 480 * (float)rand() / (float)RAND_MAX;
		ray_angle = 0.0;
		/* regular ray distribution */
		for(r = 0; r < nrays; ++r) {
			ray_angle += ray_angle_inc;
			ray.direction.x = sin_cache((int)ray_angle);
			ray.direction.y = cos_cache((int)ray_angle);

			closest = FLT_MAX;
			for(t = 0; t < N_TREES; ++t)
				//for(b = 0; b < trees[t].n_branches; ++b) {
				for(b = 0; b < trees[t].n_leaves; ++b) {
					leaf_no = trees[t].leaves[b];
					absolute_branch(&trees[t], leaf_no, &abs_leaf);
					if(leaf_ray_intersect(&abs_leaf, &ray, &itsec)) {
						distance = dist(&ray.origin, &itsec);
						if(distance < closest) {
							closest = distance;
							closest_tree = t;
							leaf = 1;//is_leaf(&trees[t], b);
						}
					}
				}
			
			if(leaf && closest < FLT_MAX) {
				/*printf("%i, ", closest_tree);*/
				trees[closest_tree].score++;
			}
		}
	}
}

void iterate_forest(struct tree trees[N_TREES])
{
	const int nrays = 100, nlights = 100;
	int i;

	light_trees(trees, nlights, nrays);

	for(i = 0; i < N_TREES; ++i) {
		trees[i].score -= 
			MAX(1, 
					trees[i].n_leaves * 0.25 + 
					(trees[i].n_branches - trees[i].n_leaves) * 1.00);

    if(trees[i].score >= trees[i].next_score) {
      trees[i].iterations++;
      expand_rule(trees[i].expansion, &trees[i].exp_size, &trees[i].seed);
      gen_branches(&trees[i]);
      trees[i].next_score = 1 + trees[i].next_score * 2;
    }
	}

	/* if your score is zero, you DIE */
	/* If a tree dies it gets randomly re-initialized */
	for(i = 0; i < N_TREES; ++i) {
		//printf("trees[%i].score = %i\n", i, trees[i].score);
		if(trees[i].score <= 0) {
			/*if(n_dead_trees < N_TREES)
				dead_trees[n_dead_trees++] = i;*/
			//printf("trees[%i] died\n", i);
			reset_tree(&trees[i]);
			init_sapling(&trees[i]);
			gen_branches(&trees[i]);
		}
	}
}

void breed_forest(struct tree trees[N_TREES])
{
  int replace, parent1, parent2, child;
	float weights[N_TREES], inverted_weights[N_TREES];

  generate_weights(trees, N_TREES, weights);
	invert_weights(weights, N_TREES, inverted_weights);
  for(replace = 0; replace < (int)(N_TREES * 0.125); ++replace) {
    /* Select the parents and a place for the child */
		child = roulette_select(inverted_weights, N_TREES);
    
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
  int w, h, wc, hc;

  w = h = TREE_SPACE;
  for(wc = 0; wc < WIDTH; ++wc)
    for(hc = 0; hc < HEIGHT; ++hc) {
      trees[wc * HEIGHT + hc].pos.x = (float)(TREE_SPACE + wc * w);
      trees[wc * HEIGHT + hc].pos.y = (float)(TREE_SPACE + hc * h);
    }
  return draw_trees(trees, N_TREES, screen);
}
