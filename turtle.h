#ifndef TURTLE_H
#define TURTLE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "types.h"
#include "geometry.h"

#define DRAW_CHAR '!'
#define PUSH_CHAR '('
#define POP_CHAR ')'
#define TURN_CL '+'
#define TURN_ACL '-'


void gen_lines(struct tree* tree);
int count_lines(struct tree* tree);
void find_leaves(struct tree *tree);

void print_lines(struct tree* tree);
void print_leaves(struct tree* tree);

void walk(struct turtle* turt);

#endif
