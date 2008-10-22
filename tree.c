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

int leaf_ray_intersect(struct line *leaf, struct ray *ray, struct point *p)
{
	/* if there is no intersection, return 0 */
	struct line ray_vec;
	ray_vec.start = ray->origin;
	ray_vec.end = ray->direction;

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

/* Finds the nearest leaf that will catch the ray,
   returns its index,
   if no leaf catches ray, returns -1
*/
int leaf_catches_ray(struct tree *tree, struct ray *ray)
{
  float closest = 100000000;
  int closest_leaf = -1;
  int distance;
  int i;
  struct point itsec;

  for(i=0; i < tree->n_leaves; ++i)
    if(ray_intersects(ray, tree->branches + tree->leaves[i], &itsec)) {
      distance = dist(&tree->pos, &itsec);
      if(distance < closest) {
				closest = distance;
				closest_leaf = i;
			}
    }

  /* Record the intersection point for drawing */
  ray->direction.x = itsec.x;
  ray->direction.y = itsec.y;
    
  return closest_leaf;
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
