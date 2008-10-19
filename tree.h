#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "symbols.h"
#include "geometry.h"

void init_tree(struct tree *tree);

void free_tree(struct tree *tree);

void reset_tree(struct tree *tree);

void randomize_tree(struct tree *tree);

void gen_rays(struct tree *tree);

int leaf_catches_ray(struct tree *tree, struct ray *ray);

int leaf_ray_intersect(struct line *leaf, struct ray *ray, struct point *p);

void absolute_branch(struct tree *tree, int leaf, struct line *abs_leaf);
char is_leaf(struct tree *tree, int branch);

#endif
