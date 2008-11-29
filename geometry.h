#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "types.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define FLT_DELTA 0.000000000000001
#define FLT_LARGE 10000000000000000.0

int f_eq(float a, float b);
int f_gt_eq(float a, float b);
int f_lt_eq(float a, float b);

float cos_cache(int angle);
float sin_cache(int angle);

float dist(struct point *p1, struct point *p2);

float mag(struct point *pt);
void norm(struct point *pt);

void get_equation(struct line *l, struct line_eq *eq);

void get_intersection(struct line_eq *l1, struct line_eq *l2,
		      struct point *itsec);

int ray_intersects(struct ray *ray, struct line *line, struct point *itsec);

int intersect(struct line *l1, struct line *l2, struct point *p);

void print_point(struct point *p);
