#include "tree.h"

void init_tree(struct tree *tree)
{
  tree->iterations = 0;
  tree->exp_size = 0;

  tree->branches = NULL;
  tree->n_branches = 0;
  
  tree->leaves = NULL;
  tree->n_leaves = 0;

  tree->score = 0;

  tree->pos.x = 0;
  tree->pos.y = 0;

  tree->n_rays = 0;
  tree->rays = NULL;

  /* Init expansion to "0" */
  tree->exp_size = 1;
  /*  tree->expansion = malloc(1); */

  /* I could do 
   *t1.expansion = 0;
   but this is more instructive */
  strncpy(tree->expansion, "0", 1);
  chars_to_rule(tree->expansion, 1, 1);

}

void free_tree(struct tree *tree)
{
  reset_tree(tree);

  tree->exp_size = 0;

  if(tree->branches) {  
    free(tree->branches);
    tree->branches = NULL;
  }
  tree->n_branches = 0;
  
  if(tree->leaves) {
    free(tree->leaves);
    tree->leaves = NULL;
  }
  tree->n_leaves = 0;

  free_rule_set(&tree->seed);
}

void reset_tree(struct tree *tree)
{
  tree->exp_size = 0;
  tree->expansion[0] = '\0';

  tree->iterations = 0;
  
  if(tree->branches) {  
    free(tree->branches);
    tree->branches = NULL;
  }
  tree->n_branches = 0;
  
  if(tree->leaves) {
    free(tree->leaves);
    tree->leaves = NULL;
  }
  tree->n_leaves = 0;

  if(tree->rays) {
    free(tree->rays);
    /*
    printf("freed rays\n");
    fflush(NULL);
    */
    tree->rays = NULL;
  }
  tree->n_rays = 0;
}

void randomize_tree(struct tree *tree)
{
  /* Make sure the tree's empty */
  reset_tree(tree);
  /* Generate random rules */
  random_rule_set(&tree->seed);
}

void gen_rays(struct tree *tree)
{
  int i;
  struct line vec;
  float ray_angle_inc, ray_angle = 0.0;

  if(tree->rays) {
    free(tree->rays);
    /*
    printf("freed rays\n");
    fflush(NULL);
    */
  }
  tree->rays = NULL;
  if(tree->n_rays != 0) tree->n_rays = 0;
  /*  if(tree->n_rays > 0 || tree->rays != NULL) return; */

  /* n_rays should depend on n_leaves */
  tree->n_rays = 2 * tree->iterations * tree->iterations;

  tree->rays = malloc(tree->n_rays * sizeof(struct ray));
  /*  printf("malloc rays\n");
      fflush(NULL);
  */
  if(tree->rays == NULL) {
    printf("Failed to allocate memory for rays\n");
    exit(-1);
  }

  ray_angle_inc = 360 / (float)tree->n_rays;

  /* regular ray distribution */
  for(i=0; i < tree->n_rays; ++i) {
    vec.start.x = tree->pos.x + 5 - 10 * (float)rand() / (float)RAND_MAX;
    vec.start.y = tree->pos.y + 5 - 10 * (float)rand() / (float)RAND_MAX;
    ray_angle += ray_angle_inc;
    vec.end.x = sin_cache((int)ray_angle);
    vec.end.y = cos_cache((int)ray_angle);
    get_equation(&vec, &tree->rays[i].ray_eq);
    tree->rays[i].direction = vec.end;   
    /*
    printf("Ray: %fx + %f: (%f, %f)\n", 
	   tree->rays[i].ray_eq.m, tree->rays[i].ray_eq.c,
	   tree->rays[i].direction.x, tree->rays[i].direction.y);
    */
  }

  /* random ray distribution perhaps?? */
}

int leaf_ray_intersect(struct line *leaf, struct ray *ray, struct point *p)
{
	/* if there is no intersection, return 0 */
	if(!intersect(leaf, ray, p))
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

	return 0;
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

void tree_lights(struct tree *trees, int ntrees)
{
  float closest = 100000000.0;
  int closest_tree = -1;
  int distance;
  struct point itsec;
	int i, j, k, r;

	for(i = 0; i < ntrees; ++i) {
		gen_rays(&trees[i]);
		for(j = 0; j < ntrees; ++j)
			for(k = 0; k < trees->n_leaves; ++k) 
				for(r = 0; r < trees->n_rays; ++r) {
					ray_intersects(&trees[i].rays[r],
												 trees[j].branches + trees[j].leaves[k], &itsec);
					distance = dist(&trees[i].pos, &itsec);
					if(distance < closest) {
						closest = distance;
						closest_tree = j;
					}
				}
		trees[closest_tree].score++;
	}
	
}

#define MAX_HITS_PER_LEAF 5
void score_tree(struct tree *tree)
{

  int i, hits = 0, leaf = -1;
  static int leaf_scores[MAX_EXPANSION_SIZE];
  for(i=0; i < tree->n_leaves; ++i) leaf_scores[i] = 0;

  gen_rays(tree);
  for(i=0; i < tree->n_rays; ++i) {
    leaf = leaf_catches_ray(tree, tree->rays + i);
    /* printf("Hit leaf %i\n", leaf); */
    if(leaf >= 0 && !(f_eq(tree->rays[i].direction.x, 0.0) &&
		      f_eq(tree->rays[i].direction.y, 0.0)))
      if(leaf_scores[leaf]++ < MAX_HITS_PER_LEAF) 
	hits++;
  }
  
  /*
  printf("Leaves, Branches, Rays, Hits: %i, %i, %i, %i\n", 
	 tree->n_leaves, tree->n_branches, tree->n_rays, hits);
  */
  /*
  if(tree->n_leaves > 0)
    print_syms(tree->expansion, tree->exp_size, tree->seed.num_rules);
  */
  /* The scoring equation */
  /* if((float)tree->n_branches * (float)tree->n_leaves > 0) */
  if((float)tree->n_leaves > 0)
    tree->score += 
      (int)((float)hits
	    - (float)tree->n_leaves / 1.5
	    - (float)tree->n_branches / 8.0
	    );
  else
    tree->score -= 1;
}
