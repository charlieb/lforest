#include "geometry.h"

/*
#define FLT_DELTA FLT_MIN
#define FLT_LARGE FLT_MAX
*/

int f_eq(float a, float b)
{
  if(fabs(a - b) <= FLT_DELTA)
    return 1;
  else 
    return 0;
}

int f_gt_eq(float a, float b)
{
  if(a > b)
    return 1;
  else if(fabs(MAX(a,b) - MIN(a,b)) <= FLT_DELTA)
    return 1;
  else 
    return 0;
}

int f_lt_eq(float a, float b)
{
  if(a < b)
    return 1;
  else if(fabs(MAX(a,b) - MIN(a,b)) <= FLT_DELTA)
    return 1;
  else 
    return 0;
}

float cos_cache(int angle)
{
  static float table[360];
  static char init = 0;
  int i;

  if(init == 0) {
    init = 1;
    for(i=0; i < 360; ++i) 
      table[i] = cos(i*M_PI/180);
  }

  return table[angle];
}

float sin_cache(int angle)
{
  static float table[360];
  static char init = 0;
  int i;

  if(init == 0) {
    init = 1;
    for(i=0; i < 360; ++i)
      table[i] = sin(i*M_PI/180);
  }

  return table[angle];
}

void print_point(struct point *p)
{
	printf("(%f, %f)", p->x, p->y);
}

void print_line(struct line *line)
{
	print_point(&line->start);
	printf(" -> ");
	print_point(&line->end);
	printf("\n");
}

float dist(struct point *p1, struct point *p2)
{
  struct point pd;
  pd.x = p1->x - p2->x;  
  pd.y = p1->y - p2->y;
  return (float)sqrt((double)(pd.x*pd.x + pd.y*pd.y));
}

float mag(struct point *pt) 
{
  return (float)sqrt((double)(pt->x*pt->x + pt->y*pt->y));
}

void norm(struct point *pt) 
{
  int magn = mag(pt);
  if(pt->x == 0 && pt->y == 0) return;
  pt->x /= magn;
  pt->y /= magn;
}

int intersect(struct line *l1, struct line *l2, struct point *p)
{
	float x_numer, x_denom, y_numer, y_denom;
	float x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = l1->start.x;
	y1 = l1->start.y;
	x2 = l1->end.x;
	y2 = l1->end.y;
	x3 = l2->start.x;
	y3 = l2->start.y;
	x4 = l2->end.x;
	y4 = l2->end.y;

	x_numer =	(x1*y2 - x2*y1)*(x3 - x4) - (x1 - x2)*(x3*y4 - x4*y3);
	x_denom = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

	y_numer = (x1*y2 - x2*y1)*(y3 - y4) - (y1 - y2)*(x3*y4 - x4*y3);
	y_denom = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

	if(x_denom <= 0.0 || y_denom <= 0.0) return 0;

	p->x = x_numer / x_denom;
	p->y = y_numer / y_denom;
	
	return 1;
}

void get_equation(struct line *l, struct line_eq *eq)
{
	if(f_eq(l->end.x, l->start.x)) {
    eq->m = FLT_LARGE;
    eq->c = l->end.y - l->end.x * eq->m;
  }
  else {
    eq->m = (l->end.y - l->start.y) / (l->end.x - l->start.x);
    eq->c = l->end.y - l->end.x * eq->m;
  }
}

void get_intersection(struct line_eq *l1, struct line_eq *l2, 
		      struct point *itsec)
{
  /* If their gradients are that similar then they are parallel*/
  if(f_eq(l1->m , l2->m)) {
    itsec->x = FLT_LARGE;
    itsec->y = FLT_LARGE;
  }
  else {
    itsec->x = (l2->c - l1->c) / (l1->m - l2->m);
    itsec->y = l1->m * itsec->x + l1->c;
  }
}

int ray_intersects(struct ray *ray, struct line *line, struct point *itsec)
{
  int special_case_flags = 0;
  struct point tmp_point;
  struct line_eq line_eq;

  get_equation(line, &line_eq);
  get_intersection(&ray->ray_eq, &line_eq, &tmp_point);

  /* Check that the line is in the same quadrant as the itsec */
  if(!(((f_gt_eq(ray->direction.x, 0.0) && f_gt_eq(tmp_point.x, 0.0)) ||
	(f_lt_eq(ray->direction.x, 0.0) && f_lt_eq(tmp_point.x, 0.0)))
       &&
       ((f_gt_eq(ray->direction.y, 0.0) && f_gt_eq(tmp_point.y, 0.0)) ||
	(f_lt_eq(ray->direction.y, 0.0) && f_lt_eq(tmp_point.y, 0.0)))))
    return 0;
     
  /* Special cases: */
  /* line is vertical */
  if(f_eq(line->start.x, line->end.x)) 
    special_case_flags += 1;
  /* line is horizontal */
  if(f_eq(line->start.y, line->end.y))  
    special_case_flags += 2;
    /* ray is vertical */
  if(f_eq(ray->direction.x, 0.0))
    special_case_flags += 4;
  /* ray is horizontal */
  if(f_eq(ray->direction.y, 0.0))
    special_case_flags += 8;

  switch (special_case_flags) {
  case 1: /* line verti */
    /* Just check the y end points */
    if(!(f_gt_eq(MAX(line->start.y, line->end.y), tmp_point.y) &&
	 f_lt_eq(MIN(line->start.y, line->end.y), tmp_point.y)))
      return 0;
    break;
  case 2: /* line horiz */
    /* Just check the x end points */
    if(!(f_gt_eq(MAX(line->start.x, line->end.x), tmp_point.x) &&
	 f_lt_eq(MIN(line->start.x, line->end.x), tmp_point.x)))
      return 0;
    break;
  case 5: /* ray verti + line verti */
    return 0;
    break;
  case 10:/* ray horiz + line horiz */
    break;
    return 0;
  default:
    if(!(f_gt_eq(MAX(line->start.x, line->end.x), tmp_point.x) && 
	 f_lt_eq(MIN(line->start.x, line->end.x), tmp_point.x) 
	 && 
	 f_gt_eq(MAX(line->start.y, line->end.y), tmp_point.y) && 
	 f_lt_eq(MIN(line->start.y, line->end.y), tmp_point.y)))
      return 0;
    break;
  }

  *itsec = tmp_point;
  return 1;
}
