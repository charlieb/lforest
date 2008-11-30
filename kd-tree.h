#ifndef KD_TREE_H
#define KD_TREE_H

#include <stdio.h>
#include <stdlib.h>

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

#endif
