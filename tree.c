#include "tree.h"

void init_tree(struct tree *tree)
{
  tree->seed.rule_size = 7;
  tree->seed.num_rules = 5;
  init_rule_set(&tree->seed);

  tree->iterations = 0;
  tree->exp_size = 0;

  tree->branches = NULL;
  tree->n_branches = 0;
  
  tree->leaves = NULL;
  tree->n_leaves = 0;
	tree->leaf_space = 0;

  /* Init expansion to "0" */
	memset(tree->expansion, 0, MAX_EXPANSION_SIZE);
  tree->exp_size = 1;
}

void free_tree(struct tree *tree)
{
  reset_tree(tree);
	free(tree->leaves);
  tree->exp_size = 0;
  free_rule_set(&tree->seed);
}

void reset_tree(struct tree *tree)
{
	tree->exp_size = 1;
	memset(tree->expansion, 0, MAX_EXPANSION_SIZE);
  tree->iterations = 0;
  
  if(tree->branches) {  
    free(tree->branches);
    tree->branches = NULL;
  }
  tree->n_branches = 0;
	tree->n_leaves = 0;

  tree->score = 0;
}

void iterate_tree(struct tree *tree, int its)
{
	for(int i = 0; i < its; ++i) {
		tree->iterations++;
		expand_rule(tree->expansion, &tree->exp_size, &tree->seed);
		tree->next_score = 1 + tree->next_score * 2;
	}
	gen_branches(tree);
}

void reset_to_random_tree(struct tree *tree)
{
	reset_tree(tree);
  random_rule_set(&tree->seed);
}

int leaf_ray_intersect(struct line *leaf, struct ray *ray, struct point *p)
{
	/* if there is no intersection, return 0 */
	struct line ray_vec;
	ray_vec.start = ray->origin;
	ray_vec.end.x = ray->origin.x + ray->direction.x;
	ray_vec.end.y = ray->origin.y + ray->direction.y;

	if(!intersect(leaf, &ray_vec, p)) 
		return 0;

	/* check to see if the intersection is on the right end
		 of the ray */
	if(ray->direction.x > 0) {
		if(p->x < ray->origin.x)
			return 0;
	}
	else {
		if(p->x > ray->origin.x)
			return 0;
	}

	if(ray->direction.y > 0) {
		if(p->y < ray->origin.y)
			return 0;
	}
	else {
		if(p->y > ray->origin.y)
			return 0;
	}

	/* check the intersection is within the leaf */
	if(leaf->start.x < leaf->end.x) {
		if(leaf->start.x > p->x || p->x > leaf->end.x)
			return 0;
	}
	else {
		if(leaf->start.x < p->x || p->x < leaf->end.x)
			return 0;
	}

	if(leaf->start.y < leaf->end.y) {
		if(leaf->start.y > p->y || p->y > leaf->end.y)
			return 0;
	}
	else {
		if(leaf->start.y < p->y || p->y < leaf->end.y)
			return 0;
	}

	return 1;
}

char is_leaf(struct tree *tree, int branch)
{
	int i;
	for(i = 0; i < tree->n_leaves; ++i)
		if(branch == tree->leaves[i])
			return 1;
	return 0;
}

void absolute_branch(struct tree *tree, int branch, struct line *abs_branch)
{
	*abs_branch = tree->branches[branch];
	abs_branch->start.x += tree->pos.x;
	abs_branch->start.y += tree->pos.y;
	abs_branch->end.x += tree->pos.x;
	abs_branch->end.y += tree->pos.y;
}

void abs_branch(struct tree *tree, struct line *branch, struct line *abs_branch)
{
	abs_branch->start.x = branch->start.x + tree->pos.x;
	abs_branch->start.y = branch->start.y + tree->pos.y;
	abs_branch->end.x = branch->end.x + tree->pos.x;
	abs_branch->end.y = branch->end.y + tree->pos.y;
}


int terminal_tree(struct tree *tree)
{
	return is_terminal(tree->expansion, tree->exp_size, tree->seed.num_rules);
}

void write_tree(FILE *file, struct tree *tree)
{
	fwrite((void*)&tree->pos, sizeof(struct point), 1, file);
	fwrite((void*)&tree->seed.rule_size, sizeof(int), 1, file);
	fwrite((void*)&tree->seed.num_rules, sizeof(int), 1, file);
	fwrite((void*)tree->seed.rules, 
				 tree->seed.num_rules * tree->seed.rule_size, 1, file);
	fwrite((void*)&tree->iterations, sizeof(int), 1, file);
	fwrite((void*)&tree->score, sizeof(int), 1, file);
	fwrite((void*)&tree->next_score, sizeof(int), 1, file);
	/*
	print_point(&tree->pos);
	printf(": %i / %i\n", tree->score, tree->next_score);
	print_rule_set(&tree->seed);
	*/
}

void read_tree(FILE *file, struct tree *tree)
{
	int i;

	reset_tree(tree);

	fread((void*)&tree->pos, sizeof(struct point), 1, file);
	fread((void*)&tree->seed.rule_size, sizeof(int), 1, file);
	fread((void*)&tree->seed.num_rules, sizeof(int), 1, file);
	init_rule_set(&tree->seed);
	fread((void*)tree->seed.rules, 
				 tree->seed.num_rules * tree->seed.rule_size, 1, file);
	fread((void*)&tree->iterations, sizeof(int), 1, file);
	fread((void*)&tree->score, sizeof(int), 1, file);
	fread((void*)&tree->next_score, sizeof(int), 1, file);

	for(i = 0; i < tree->iterations; ++i)
		expand_rule(tree->expansion, &tree->exp_size, &tree->seed);
	gen_branches(tree);
	/*
	print_point(&tree->pos);
	printf(": %i / %i\n", tree->score, tree->next_score);
	print_rule_set(&tree->seed);
	*/

}

void print_tree(struct tree *tree)
{
	printf("Pos: "); print_point(&tree->pos); printf("\n");
	printf("Score %i / %i\n", tree->score, tree->next_score);
	printf("Iterations: %i\n", tree->iterations);
	printf("Rule:\n");
	print_rule_set(&tree->seed);
	printf("Exp: "); print_syms(tree->expansion, tree->exp_size, tree->seed.num_rules);
	printf("\n");
	printf("Leaves/Branches: %i/%i\n", tree->n_leaves, tree->n_branches);
	printf("-------\n");
}
