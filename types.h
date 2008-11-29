#ifndef L_SYS_TYPES_H
#define L_SYS_TYPES_H

struct config {
	int nrays;
	int nlights;
	int screen_width;
	int screen_height;
	int forest_width;
	int forest_height;
	int tree_space;
	float branch_cost;
	float leaf_cost;
	
	
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

#define MAX_EXPANSION_SIZE 1000
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
  struct line_eq ray_eq;
	struct point origin;
  struct point direction;
};

#endif
