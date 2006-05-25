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

void randomize_tree(struct tree *tree)
{
  /* Make sure the tree's empty */
  free_tree(tree);
  init_tree(tree);

  /* Generate random rules */
  init_rule_set(&tree->seed);
  random_rule_set(&tree->seed);
}


void gen_rays(struct tree *tree)
{
  int i;
  struct line vec;
  float ray_angle_inc, ray_angle = 0.0;

  if(tree->n_rays > 0 || tree->rays != NULL) return;

  /* n_rays should depend on n_leaves */
  tree->n_rays = tree->n_leaves;/* * tree->n_leaves;*/
  tree->rays = malloc(tree->n_rays * sizeof(struct ray));
  if(tree->rays == NULL) {
    printf("Failed to allocate memory for rays\n");
    exit(-1);
  }

  ray_angle_inc = 360 / (float)tree->n_rays;

  /* regular ray distribution */
  for(i=0; i < tree->n_rays; ++i) {
    vec.start.x = tree->pos.x;
    vec.start.y = tree->pos.y;
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
      if(distance < closest) closest_leaf = i;
    }

  /* Record the intersection point for drawing */
  ray->direction.x = itsec.x;
  ray->direction.y = itsec.y;
    
  return closest_leaf;
}

#define MAX_HITS_PER_LEAF 10

void score_tree(struct tree *tree)
{

  int i, hits = 0, leaf = -1;
  static int leaf_scores[MAX_EXPANSION_SIZE];
  for(i=0; i < tree->n_leaves; ++i) leaf_scores[i] = 0;

  if(tree->rays) free(tree->rays);
  tree->rays = NULL;
  if(tree->n_rays != 0) tree->n_rays = 0;

  gen_rays(tree);
  for(i=0; i < tree->n_rays; ++i) {
    leaf = leaf_catches_ray(tree, tree->rays + i);
    /* printf("Hit leaf %i\n", leaf); */
    if(leaf >= 0)
      if(leaf_scores[leaf]++ < MAX_HITS_PER_LEAF) 
	hits++;
  }
 
  printf("Leaves, Branches, Rays, Hits: %i, %i, %i, %i\n", 
	 tree->n_leaves, tree->n_branches, tree->n_rays, hits);
  if(tree->n_leaves > 0)
    print_syms(tree->expansion, tree->exp_size, tree->seed.num_rules);

  /* The scoring equation */
  if((float)tree->n_branches * (float)tree->n_leaves > 0)
    tree->score = 
      (int)((float)hits
	    - (float)tree->n_leaves / 32.0 
	    - (float)tree->n_branches / 64.0 
	    - (float)tree->exp_size / 64.0);
}


