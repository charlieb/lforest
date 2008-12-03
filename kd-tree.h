#ifndef KD_TREE_H
#define KD_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "types.h"
#include "geometry.h"
#include "tree.h"

struct kd_node {
	struct node *node;
	int dim;
	struct kd_node *low;
	struct kd_node *high;
};

struct node {
	struct point pt;
	struct tree *tree;
	struct line *branch;
};

void nodes_from_trees(struct tree *trees, int ntrees,
											struct node **nodes, int *nnodes);
void sort_nodes(struct node **xnodes, struct node **ynodes,
								struct node *nodes, int nnodes);

void build_kd_tree(struct node **xnodes, struct node **ynodes, int nnodes, int dim,
									 struct kd_node *current);

struct node *nearest_by_tree(struct kd_node *root, struct point *pt);
int nearest_by_tree_range(struct kd_node *root, struct point *pt, float range,
													struct node ***in_range, int *nin_range);

void free_kd_tree(struct kd_node *tree);

void print_kd_tree(struct kd_node *root);
void print_node(struct node *node);
void test_kd_tree();
#endif
