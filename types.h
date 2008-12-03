#ifndef L_SYS_TYPES_H
#define L_SYS_TYPES_H

struct config {
	
	int ntrees;
	int nrays;
	int width;
	int height;
	
	int init_score;
	int init_iterations;

	float branch_cost;
	float leaf_cost;
	/* tree repalcements /1.0 */
	float replace_trees;
	/* chance of tree being randomized instead of bread */
	float re_init_chance;
};

struct point {
  float x;
  float y;
};

struct line {
  struct point start;
  struct point end;
};

struct line_eq{
  float m, c;
};

struct rule_set {
  int rule_size;
  int num_rules;
  char *rules;
};

#define MAX_EXPANSION_SIZE 10000
struct tree {
  struct point pos;

  struct rule_set seed;

  int iterations;
  char expansion[MAX_EXPANSION_SIZE];
  int exp_size;

  struct line *branches;
	/* n_branches is the total number of branches: leaves and non-leaves alike */
  int n_branches;
  /* leaves => branches[leaves[n]] */
  int *leaves;
	/* n_leaves is the number of branches that are leaves */
  int n_leaves;
	/* amount of memory allocated to leaves */
	int leaf_space;

  int score;
	int next_score;
};

struct turtle {
  float x, y;
  int angle;
};

struct ray {
	struct point origin;
  struct point direction;
};

struct forest {
	/* config contains n* varaibles e.g. ntrees, nrays etc. */
	struct config config;

	struct tree *trees;

	/* NB: these are lines for drawing the rays not the rays themselves */
	struct line *ray_lines;
};

#endif
