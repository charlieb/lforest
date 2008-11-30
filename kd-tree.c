#include "kd-tree.h"

void nodes_from_trees(struct tree *trees, int ntrees,
											struct node **nodes, int *nnodes)
{
	int t, b;
	const int node_block_size = 100;
	int nnodes_alloced = 0;
	struct line abs_branch;

	*nodes = NULL;
	*nnodes = 0;

	for(t = 0; t < ntrees; ++t)
		for(b = 0; b < trees[t].n_branches; ++b) {
			/* allocate more nodes if necessary */
			if(*nnodes == nnodes_alloced) {
				nnodes_alloced += node_block_size;
				*nodes = realloc(*nodes, nnodes_alloced * sizeof(struct node));
				if(NULL == *nodes) {
					printf("nodes_from_trees: Failed to allocate node memory\n");
					exit(-1);
				}
			}

			(*nodes)[*nnodes].tree = &trees[t];
			(*nodes)[*nnodes].branch = &trees[t].branches[b];

			absolute_branch(&trees[t], b, &abs_branch);
			bisect_line(&(*nodes)[*nnodes].pt, &abs_branch);
			(*nnodes)++;
		}

	/* free extra memory */
	*nodes = realloc(*nodes, *nnodes * sizeof(struct node));
}


static int xpoint(const void *p1, const void *p2)
{
	return ((*(struct node **)p1))->pt.x > ((*(struct node **)p2))->pt.x ? 1 : -1;
}

static int ypoint(const void *p1, const void *p2)
{
	return ((*(struct node **)p1))->pt.y > ((*(struct node **)p2))->pt.y ? 1 : -1;
}

void sort_nodes(struct node **xnodes, struct node **ynodes,
								struct node *nodes, int nnodes)
{
	int i;
	
	for(i = 0; i < nnodes; ++i) 
		xnodes[i] = ynodes[i] = &nodes[i];
	
	qsort((void*)xnodes, nnodes, sizeof(struct node*), xpoint);
	qsort((void*)ynodes, nnodes, sizeof(struct node*), ypoint);		
}

void xpartition(struct node **nodes, int nnodes, int center,
								struct node **nodes_part, int *nlo_nodes, int *nhi_nodes)
{
	int i, hi_start;
	*nlo_nodes = hi_start = *nhi_nodes = 0;

	for(i = 0; i < nnodes; ++i) 
		if(center == i) 
			continue;
		else if(nodes[i]->pt.x > nodes[center]->pt.x)
			nodes_part[++(*nhi_nodes) - 1] = nodes[i];
		else {
			nodes_part[++(*nlo_nodes) - 1] = nodes[i];
			if(*nlo_nodes == hi_start) {
				hi_start++;
				memmove(&nodes_part[hi_start], nodes_part[hi_start-1], 
								*nhi_nodes * sizeof(struct node*));
			}
		}
}

void ypartition(struct node **nodes, int nnodes, int center,
								struct node **nodes_part, int *nlo_nodes, int *nhi_nodes)
{
	int i, hi_start;
	*nlo_nodes = hi_start = *nhi_nodes = 0;
	for(i = 0; i < nnodes; ++i) 
		if(center == i) 
			continue;
		else if(nodes[i]->pt.y > nodes[center]->pt.y)
			nodes_part[++(*nhi_nodes) - 1] = nodes[i];
		else {
			nodes_part[++(*nlo_nodes) - 1] = nodes[i];
			if(*nlo_nodes == hi_start + 1) {
				hi_start++;
				if(*nhi_nodes > 0)
					memmove(&nodes_part[hi_start], nodes_part[hi_start-1], 
									*nhi_nodes * sizeof(struct node*));
			}
		}
}


void build_kd_tree(struct node **xnodes, struct node **ynodes, int nnodes, int dim,
									 struct kd_node *current)
{
	struct node **next_xnodes_high = NULL;
	struct node **next_xnodes_low = NULL;
	struct node **next_ynodes_high = NULL;
	struct node **next_ynodes_low = NULL;	
	struct node **next_xnodes = NULL;
	struct node **next_ynodes = NULL;

	int nh, nl, node_idx;
	current->dim = dim;

	if(2 <= nnodes) {
		current->node = xnodes[0];
		current->high = malloc(sizeof(struct kd_node));
		current->high->node = xnodes[1];
		current->high->dim = !dim;
		current->high->high = current->high->low = NULL;
		return;
	}

	if(1 == dim) {
		node_idx = nnodes / 2;
		current->node = xnodes[node_idx];

		next_xnodes_low = xnodes;
		nl = node_idx -1;

		next_xnodes_high = &xnodes[node_idx + 1];
		nh = nnodes - node_idx;

		next_ynodes = malloc((nnodes - 1) * sizeof(struct node*));

		xpartition(ynodes, nnodes, node_idx, next_ynodes, &nl, &nh);
		next_ynodes_low = next_ynodes;
		next_ynodes_high = &next_ynodes[nh];
	}
	else {
		node_idx = nnodes / 2;
		current->node = ynodes[node_idx];

		next_ynodes_low = xnodes;
		nl = node_idx -1;

		next_ynodes_high = &ynodes[node_idx + 1];
		nh = nnodes - node_idx;

		next_xnodes = malloc((nnodes - 1) * sizeof(struct node*));

		ypartition(xnodes, nnodes, node_idx, next_xnodes,&nl, &nh);
		next_xnodes_low = next_xnodes;
		next_xnodes_high = &next_xnodes[nh];
	}

	current->low = malloc(sizeof(struct kd_node));
	current->high = malloc(sizeof(struct kd_node));
	
	build_kd_tree(next_xnodes_low, next_ynodes_low, nl, !dim, 
								current->low);
	build_kd_tree(next_xnodes_high, next_ynodes_high, nh, !dim, 
								current->high);

	if(1 == dim) 
		free(next_ynodes);
	else
		free(next_xnodes);
}

