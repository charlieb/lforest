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

void xpartition(struct node **nodes, int nnodes, struct node *center,
								struct node **nodes_part, int *nlo_nodes, int *nhi_nodes)
{
	int i, hi_start;
	*nlo_nodes = hi_start = *nhi_nodes = 0;
	for(i = 0; i < nnodes; ++i) 
		if(center == nodes[i])
			continue;
		else if(nodes[i]->pt.x > center->pt.x)
			nodes_part[++(*nhi_nodes) + hi_start - 1] = nodes[i];
		else {
			/* move all the high nodes along one */
			hi_start++;
			if(*nhi_nodes > 0)
				memmove(&nodes_part[hi_start], &nodes_part[hi_start-1], 
								*nhi_nodes * sizeof(struct node*));
			nodes_part[++(*nlo_nodes) - 1] = nodes[i];
		}
}

void ypartition(struct node **nodes, int nnodes, struct node *center,
								struct node **nodes_part, int *nlo_nodes, int *nhi_nodes)
{
	int i, hi_start;
	*nlo_nodes = hi_start = *nhi_nodes = 0;
	for(i = 0; i < nnodes; ++i) 
		if(center == nodes[i]) 
			continue;
		else if(nodes[i]->pt.y > center->pt.y)
			nodes_part[++(*nhi_nodes) + hi_start - 1] = nodes[i];
		else {
			hi_start++;
			if(*nhi_nodes > 0)
				memmove(&nodes_part[hi_start], &nodes_part[hi_start-1], 
								*nhi_nodes * sizeof(struct node*));
			nodes_part[++(*nlo_nodes) - 1] = nodes[i];			
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

	printf("-> %i nodes: %c\n", nnodes, (1 == dim ? 'x' : 'y'));
	for(int i = 0; i < nnodes; ++i) {
		printf("(%f, %f)  (%f, %f)\n", 
					 xnodes[i]->pt.x, xnodes[i]->pt.y,
					 ynodes[i]->pt.x, ynodes[i]->pt.y);
		fflush(NULL);
	}

	memset(current, 0, sizeof(struct kd_node));
	current->dim = dim;
	
	if(1 == nnodes) {
		current->high = current->low = NULL;
		current->node = xnodes[0];
		printf("<- %i nodes\n", nnodes);
		fflush(NULL);
		return;
	}

	node_idx = nnodes / 2;
	next_ynodes = malloc((nnodes - 1) * sizeof(struct node*));
	next_xnodes = malloc((nnodes - 1) * sizeof(struct node*));

	if(1 == dim) {
		current->node = xnodes[node_idx];

		xpartition(ynodes, nnodes, current->node, next_ynodes, &nl, &nh);
		xpartition(xnodes, nnodes, current->node, next_xnodes, &nl, &nh);

	}
	else {
		current->node = ynodes[node_idx];

		ypartition(xnodes, nnodes, current->node, next_xnodes, &nl, &nh);
		ypartition(ynodes, nnodes, current->node, next_ynodes, &nl, &nh);
	}

	printf("Mid: (%f, %f)\nLo: %i, M: %i, Hi: %i\n", 
				 xnodes[node_idx]->pt.x, xnodes[node_idx]->pt.y,
				 nl, node_idx, nh);
	fflush(NULL);
	
	next_xnodes_low = next_xnodes;
	next_xnodes_high = &next_xnodes[nl];
	
	next_ynodes_low = next_ynodes;
	next_ynodes_high = &next_ynodes[nl];
	
	for(int i = 0; i < nl; ++i) {
		printf("L: (%f, %f)  (%f, %f)\n", 
					 next_xnodes_low[i]->pt.x, next_xnodes_low[i]->pt.y,
					 next_ynodes_low[i]->pt.x, next_ynodes_low[i]->pt.y);
		fflush(NULL);
	}

	for(int i = 0; i < nh; ++i) {
		printf("H: (%f, %f)  (%f, %f)\n", 
					 next_xnodes_high[i]->pt.x, next_xnodes_high[i]->pt.y,
					 next_ynodes_high[i]->pt.x, next_ynodes_high[i]->pt.y);
		fflush(NULL);
	}

	if(nl > 0) {
		current->low = malloc(sizeof(struct kd_node));
	
		printf("L\n"); fflush(NULL);
		build_kd_tree(next_xnodes_low, next_ynodes_low, nl, !dim, 
									current->low);
	}

	if(nh > 0) {
		current->high = malloc(sizeof(struct kd_node));
		printf("H\n"); fflush(NULL);
		build_kd_tree(next_xnodes_high, next_ynodes_high, nh, !dim, 
									current->high);
	}

	free(next_ynodes);
	free(next_xnodes);

	printf("<- %i nodes\n", nnodes); fflush(NULL);
}

struct node *nearest_naieve(struct node *nodes, int nnodes, struct point *pt)
{
	int i, smallest;
	float smallest_dist = FLT_MAX;
	float d;
	for(i = 0; i < nnodes; ++i) {
		d = dist(&nodes[i].pt, pt);
		if(smallest_dist > d) {
			smallest = i;
			smallest_dist = d;
		}
	}
	return &nodes[smallest];
}

void nearest_rec(struct kd_node *tree, struct point *pt,
								 float *min_dist, struct node **nearest)
{
	float d;

	if(NULL == tree) return;

	d = dist(&tree->node->pt, pt);
	if(d < *min_dist) {
		*nearest = tree->node;
		*min_dist = d;
	}

	d = (1 == tree->dim ? tree->node->pt.x - pt->x : tree->node->pt.y - pt->y);
	
	if (fabs(d) > *min_dist) {
		if(1 == tree->dim ? tree->node->pt.x >= pt->x : tree->node->pt.y >= pt->y)
			nearest_rec(tree->low, pt, min_dist, nearest);
		else
			nearest_rec(tree->high, pt, min_dist, nearest);
	}
	// If the distance to the target is 
	// less than the nearest distance, we still need to look
	// in both directions.
	else {
		nearest_rec(tree->low, pt, min_dist, nearest);
		nearest_rec(tree->high, pt, min_dist, nearest);
	}
}

struct node *nearest_by_tree(struct kd_node *root, struct point *pt)
{
	struct node *nearest_node = root->node;
	float min_dist = FLT_MAX;

	nearest_rec(root, pt, &min_dist, &nearest_node);
	return nearest_node;
}

void print_kd_tree(struct kd_node *root)
{
	if(NULL == root) return;

	printf("print_kd_tree: (%f, %f) : %p\n", 
				 root->node->pt.x, root->node->pt.y, (void*)root->node);
	print_kd_tree(root->low);	
	print_kd_tree(root->high);
}
