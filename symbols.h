#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "types.h"

void init_rule_set(struct rule_set *rules);

void free_rule_set(struct rule_set *rules);

char* get_rule(int rule_no, struct rule_set *rules);

void print_syms(char* syms, int syms_size, int max_num);

void print_rule_set(struct rule_set *rules);

void chars_to_rule(char *chars, int length, int max_rule);

int expansion_length(char *exp, int exp_size, struct rule_set *rule);

void expand_rule(char exp[MAX_EXPANSION_SIZE], 
		 int *exp_size, struct rule_set *rules);

void random_rule_set(struct rule_set *rules);

#endif
