#include "forest.h"

void init_sapling(struct forest *forest, struct tree *tree)
{
	reset_tree(tree);
	tree->score = forest->config.init_score;
	iterate_tree(tree, forest->config.init_iterations);
	tree->next_score = 1 + tree->score * 2;

	tree->pos.x = (int)(forest->config.width * (float)random() / RAND_MAX);
	tree->pos.y = (int)(forest->config.height * (float)random() / RAND_MAX);
}


void init_random_sapling(struct forest *forest, struct tree *tree)
{
	reset_to_random_tree(tree);
	tree->score = forest->config.init_score;
	iterate_tree(tree, forest->config.init_iterations);
	tree->next_score = 1 + tree->score * 2;

	tree->pos.x = (int)(forest->config.width * (float)random() / RAND_MAX);
	tree->pos.y = (int)(forest->config.height * (float)random() / RAND_MAX);
}

void alloc_forest(struct forest *forest)
{
	pthread_mutex_init(&forest->trees_mtx, NULL);
	pthread_mutex_init(&forest->ray_lines_mtx, NULL);

	forest->trees = malloc(forest->config.ntrees * sizeof(struct tree));
	memset(forest->trees, 0, forest->config.ntrees * sizeof(struct tree));

	forest->ray_lines = malloc(forest->config.nrays * sizeof(struct line));
	memset(forest->ray_lines, 0, forest->config.nrays * sizeof(struct line));
}

void init_forest(struct forest *forest)
{
  int i;
	alloc_forest(forest);

	for(i=0; i < forest->config.ntrees; ++i) {
		init_tree(&forest->trees[i]);
		init_random_sapling(forest, &forest->trees[i]);
	}
}

int closest_hit(struct tree *trees, int ntrees, struct ray *ray)
{
	float closest, distance;
	int closest_tree = -1, leaf_no, t, b;
	struct line abs_leaf;
	struct point itsec;

	closest = FLT_MAX;
	for(t = 0; t < ntrees; ++t)
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

int closest_hit_by_tree(struct forest *forest,
												struct kd_node *kd_tree, struct ray *ray,
												struct line *ray_line)
{
	struct line abs_leaf;
	struct point itsec, ray_test, closest_itsec;

	float closest = FLT_MAX, distance;

	struct node **near_nodes = NULL;
	int nnear_nodes = 0;
	struct node *nearest_node  = NULL;

	const float inc_size = 10.0;

	closest_itsec.x = closest_itsec.y = 0.0;

	ray_test = ray->origin;
	while(ray_test.x > 0 && ray_test.x < forest->config.width &&
				ray_test.y > 0 && ray_test.y < forest->config.height) {
		nearest_by_tree_range(kd_tree, &ray_test, inc_size, &near_nodes, &nnear_nodes);
		for(int i = 0; i < nnear_nodes; ++i) {
			abs_branch(near_nodes[i]->tree, near_nodes[i]->branch, &abs_leaf);
			if(leaf_ray_intersect(&abs_leaf, ray, &itsec)) {
				distance = dist(&ray->origin, &itsec);
				if(distance < closest) {
					closest = distance;
					nearest_node = near_nodes[i];
					closest_itsec = itsec;
				}
			}
		}
		free(near_nodes);
		near_nodes = NULL;
		nnear_nodes = 0;
		if(NULL != nearest_node) {
			pthread_mutex_lock(&forest->ray_lines_mtx);
			ray_line->start = ray->origin;
			ray_line->end = closest_itsec;
			pthread_mutex_unlock(&forest->ray_lines_mtx);
			if(is_leaf(nearest_node->tree, 
								 nearest_node->branch - nearest_node->tree->branches))
				return nearest_node->tree - forest->trees;
			else /* we hit a branch, no points for you */
				return -1;
		}

		ray_test.x += ray->direction.x * inc_size;
		ray_test.y += ray->direction.y * inc_size;
	}

	return -1;
}


void light_trees(struct forest *forest)
{
	struct ray ray;
	float ray_angle;
	int closest_tree = 0;
	int r;
	struct kd_node kd_tree;

	struct node *nodes;
	int nnodes;
	struct node **xnodes = NULL, **ynodes = NULL;

	nodes_from_trees(forest->trees, forest->config.ntrees, &nodes, &nnodes);
	if(nnodes <= 0) return;
	ynodes = malloc(nnodes * sizeof(struct node*));
	xnodes = malloc(nnodes * sizeof(struct node*));
	sort_nodes(xnodes, ynodes, nodes, nnodes);
	build_kd_tree(xnodes, ynodes, nnodes, 1, &kd_tree);

	free(xnodes);
	free(ynodes);

	pthread_mutex_lock(&forest->ray_lines_mtx);
	memset(forest->ray_lines, 0, forest->config.nrays * sizeof(struct line));
	pthread_mutex_unlock(&forest->ray_lines_mtx);

	for(r = 0; r < forest->config.nrays; ++r) {
		ray.origin.x = forest->config.width * (float)rand() / (float)RAND_MAX;
		ray.origin.y = forest->config.height * (float)rand() / (float)RAND_MAX;
		ray_angle = 360 * (float)rand() / RAND_MAX;
		
		ray.direction.x = sin_cache((int)ray_angle);
		ray.direction.y = cos_cache((int)ray_angle);

		closest_tree = closest_hit_by_tree(forest, &kd_tree, &ray, &forest->ray_lines[r]);
		//closest_tree = closest_hit(trees, &ray);
					
		if(-1 != closest_tree) {
			forest->trees[closest_tree].score++;
		}
	}
	
	free(nodes);
	free_kd_tree(&kd_tree);
}

void iterate_forest(struct forest *forest)
{
	int i;

	/*
	for(i = 0; i < forest->config.ntrees; ++i)
		if(forest->trees[i].score <= -10)
			new_random_sapling(forest, &forest->trees[i]);
	*/

	light_trees(forest);

	for(i = 0; i < forest->config.ntrees; ++i) {
		forest->trees[i].score -= 
			MAX(1, 
					forest->trees[i].n_leaves * forest->config.leaf_cost + 
					(forest->trees[i].n_branches - forest->trees[i].n_leaves) * 
					forest->config.branch_cost);

    if(forest->trees[i].score >= forest->trees[i].next_score) {
			pthread_mutex_lock(&forest->trees_mtx);
			iterate_tree(&forest->trees[i], 1);
			pthread_mutex_unlock(&forest->trees_mtx);
		}
	}
}

void breed_forest(struct forest *forest)
{
  int i, parent1, parent2, child;
	float weights[forest->config.ntrees], inverted_weights[forest->config.ntrees];

  generate_weights(forest->trees, forest->config.ntrees, weights);
	invert_weights(weights, forest->config.ntrees, inverted_weights);

	for(i = 0; i < (int)(forest->config.ntrees * forest->config.replace_trees); ++i) {
		
		child = roulette_select(inverted_weights, forest->config.ntrees);

		if(forest->trees[child].score > 0)
			continue;
		else if(random() / (float)RAND_MAX > forest->config.re_init_chance) {
			/* some dead trees randomly re-initialized to introduce "fresh blood" */
			init_random_sapling(forest, &forest->trees[i]);
			continue;
		}
    
    parent1 = child;
    while(child == parent1)
      parent1 = roulette_select(weights, forest->config.ntrees);
    
		parent2 = child;
    while(child == parent2)
      parent2 = roulette_select(weights, forest->config.ntrees);
		
    /*parent2 = pick_neighbour(parent1, trees);*/
    /*child = pick_neighbour(parent1, trees);*/
		/*
    printf("Breeding: %i(%i) + %i(%i) = %i(%i)\n", 
	   parent1, trees[parent1].score,
	   parent2, trees[parent2].score,
	   child,   trees[child].score);
		*/
    crossover(&forest->trees[parent1], &forest->trees[parent2], 
							&forest->trees[child]);

		init_sapling(forest, &forest->trees[child]);    
  }
}

void write_forest(FILE *file, struct forest *forest)
{
	fwrite((void*)&forest->config.ntrees, sizeof(int), 1, file);
	for(int i = 0; i < forest->config.ntrees; ++i)
		write_tree(file, &forest->trees[i]);
}

void read_forest(FILE *file, struct forest *forest)
{
	fread((void*)&forest->config.ntrees, sizeof(int), 1, file);
	alloc_forest(forest);
	for(int i = 0; i < forest->config.ntrees; ++i) 
		read_tree(file, &forest->trees[i]);
}

void free_forest(struct forest *forest)
{
	for(int i = 0; i < forest->config.ntrees; ++i)
		free_tree(&forest->trees[i]);
	free(forest->trees);
	free(forest->ray_lines);
}
