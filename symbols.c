#include "symbols.h"

/* because '!' = 33 */
#define MAX_RULES 32

void init_rule_set(struct rule_set *rules)
{
  rules->rules = malloc(rules->rule_size * rules->num_rules);
}

void free_rule_set(struct rule_set *rules)
{
  if(rules->rules) free(rules->rules);
}

char* get_rule(int rule_no, struct rule_set *rules)
{
  if(rule_no > rules->num_rules) return NULL;
  return rules->rules + rule_no * rules->rule_size;
}

void print_syms(char *syms, int syms_size, int max_num)
{
  int i;
  for(i=0; i < syms_size; ++i) {
    /*    printf(" %i:", i); */
    if(syms[i] <= max_num)
      printf("%i", syms[i]);
    else
      printf("%c", syms[i]);
  }
  printf("\n");
}

void print_rule_set(struct rule_set *rules)
{
  int i;
  for(i=0; i < rules->num_rules; ++i) 
    print_syms(get_rule(i, rules), rules->rule_size, rules->num_rules);
}


int expansion_length(char *exp, int exp_size, struct rule_set *rule)
{
  int i, j;
  int rule_sizes[MAX_RULES];
  int total = 0;

  /* init */
  for(i=0; i < rule->num_rules; ++i) rule_sizes[i] = 0;

  /* count the valid chars in the each rule */
  for(i=0; i < rule->num_rules; ++i) 
    for(j=0; j < rule->rule_size; ++j)
      if(get_rule(i, rule)[j] != BLANK) 
	++rule_sizes[i];
  
  for(i=0; i < exp_size; ++i) 
    if(exp[i] <= rule->rule_size) 
      total += rule_sizes[(int)exp[i]];
    else
      total++;

  /*  printf("expansion_length => %i\n", total); */

  return total;
}

void expand_rule(char exp[MAX_EXPANSION_SIZE],
		 int *exp_size, struct rule_set *rules)
{
  int new_exp_size = expansion_length(exp, *exp_size, rules);
  char new_exp[MAX_EXPANSION_SIZE];
  char *rule = NULL;
  int i;
  int exp_pos = 0;
  int rule_pos = 0;
  int rule_len;
 
  if(*exp_size >= MAX_EXPANSION_SIZE) 
    new_exp_size = MAX_EXPANSION_SIZE;
  
  if(!new_exp) {
    printf("Failed to malloc in expand_rule\n");
    exit(-1);
  }
  
  for(i=0; i < *exp_size; ++i) 
    if(exp[i] > rules->num_rules) 
      new_exp[exp_pos++] = exp[i]; /* Not a rule, just a char */
    else {
      rule = get_rule(exp[i], rules);
      rule_len = rules->rule_size;
      rule_pos = 0;
      
      rule_len--;
      /* copy chars */
      while(rule_pos <= rule_len && exp_pos < MAX_EXPANSION_SIZE)
	new_exp[exp_pos++] = rule[rule_pos++];
    }
  
  *exp_size = new_exp_size;
  for(i=0; i < new_exp_size; ++i) exp[i] = new_exp[i];
}

void random_rule_set(struct rule_set *rules)
{
  int i;
  char rand_char;
  char special_chars[5] = "+-()!";

  for(i=0; i < rules->rule_size * rules->num_rules; ++i) {
    /* special chars +-()! */ 
    rand_char = (int)floor((5 + rules->num_rules) * 
			   (rand() / (float)RAND_MAX));
    /*printf("%i: %i", i, rand_char); */
    
    if(rand_char >= rules->num_rules)
      rules->rules[i] = special_chars[rand_char - rules->num_rules];
    else
      rules->rules[i] = rand_char;
    
    /*    if(rand_char >= rules->num_rules)
      printf("=%c\n", rules->rules[i]);
    else
      printf("=%i\n", rules->rules[i]);
    */
  }
  /*
  print_rule_set(rules);  
  */
}

void chars_to_rule(char *chars, int length, int max_rule)
{
  int i;
  char val;
  for(i=0; i < length; ++i) {
    val = chars[i] - '0';
    if(max_rule > val && val >= 0) 
      chars[i] = val;
  }    
}
