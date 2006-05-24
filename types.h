#ifndef L_SYS_TYPES_H
#define L_SYS_TYPES_H

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
  char * rules;
};

#define MAX_EXPANSION_SIZE 10000
struct tree {
  struct point pos;

  struct rule_set seed;

  int iterations;
  char expansion[MAX_EXPANSION_SIZE];
  int exp_size;

  struct line *branches;
  int n_branches;
  /* leaves => lines[leaves[n]] */
  int *leaves;
  int n_leaves;

  struct ray *rays;
  int n_rays;

  int score;
  
};

struct turtle {
  float x, y;
  int angle;
};

struct ray {
  struct line_eq ray_eq;
  struct point direction;
};

#endif
