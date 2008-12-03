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

  return table[angle % 360];
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

  return table[angle % 360];
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

void bisect_line(struct point *pt, struct line *line)
{
	pt->x = (line->start.x + line->end.x) / 2;
	pt->y = (line->start.y + line->end.y) / 2;
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
