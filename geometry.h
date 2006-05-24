#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "types.h"

float cos_cache(int angle);
float sin_cache(int angle);

float dist(struct point *p1, struct point *p2);

float mag(struct point *pt);
void norm(struct point *pt);

void get_equation(struct line *l, struct line_eq *eq);

void get_intersection(struct line_eq *l1, struct line_eq *l2,
		      struct point *itsec);

int ray_intersects(struct ray *ray, struct line *line, struct point *itsec);


