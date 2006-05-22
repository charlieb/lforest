#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "types.h"
#include "symbols.h"
#include "geometry.h"

void init_tree(struct tree *tree);
void free_tree(struct tree *tree);
void randomize_tree(struct tree *tree);
void gen_rays(struct tree *tree);
int leaf_catches_ray(struct tree *tree, struct ray *ray);
void score_tree(struct tree *tree);
void crossover(struct tree *tree1, struct tree *tree2, struct tree *offspring);


#endif
