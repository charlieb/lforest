#ifndef GENETICS_H
#define GENETICS_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tree.h"

int uniform_select(int n);

int roulette_select(float *weights, int n_weights);

void generate_weights(struct tree *trees, int n_trees, float *weights);

void crossover(struct tree *tree1, struct tree *tree2, struct tree *offspring);

#endif
