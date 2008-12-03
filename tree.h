#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "symbols.h"
#include "geometry.h"
#include "turtle.h"

void init_tree(struct tree *tree);

void free_tree(struct tree *tree);

void reset_tree(struct tree *tree);

void iterate_tree(struct tree *tree, int its);

void reset_to_random_tree(struct tree *tree);

int leaf_ray_intersect(struct line *leaf, struct ray *ray, struct point *p);

void absolute_branch(struct tree *tree, int leaf, struct line *abs_leaf);
void abs_branch(struct tree *tree, struct line *branch, struct line *abs_branch);
char is_leaf(struct tree *tree, int branch);
int terminal_tree(struct tree *tree);

void write_tree(FILE *file, struct tree *tree);
void read_tree(FILE *file, struct tree *tree);
void print_tree(struct tree *tree);

#endif
