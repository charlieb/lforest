#include "genetics.h"

int uniform_select(int n)
{
  return (int)((float)n * ((float)random() / (float)RAND_MAX));
}

int roulette_select(float *weights, int n_weights)
{
  float rnd = random() / (float)RAND_MAX;
  float total = weights[0];
  int w = 0;

  while(total < rnd && w < n_weights - 1) total += weights[++w];
  return w;
}

void invert_weights(float *weights, int nweights, float *inverted_weights)
{
	int i;
	float total = 0.0f;
	for(i = 0; i < nweights; ++i) 
		total += inverted_weights[i] = 1.0f - weights[i];
	
	for(i = 0; i < nweights; ++i)
		inverted_weights[i] /= total;
		
}

void generate_weights(struct tree *trees, int n_trees, float *weights)
{
  int i;
  float total = 0.0;
  for(i=0; i < n_trees; ++i) total += trees[i].score;
  for(i=0; i < n_trees; ++i) {
    weights[i] = (float)trees[i].score / total;
    /*    printf("score[%i] = %i : %f\n", i, trees[i].score, weights[i]); */
  }
}
void crossover(struct tree *tree1, struct tree *tree2, struct tree *offspring)
{
  int length = tree1->seed.rule_size * tree1->seed.num_rules;
  int cross_point;
  /* check that the trees are compatible */
  if(length != tree2->seed.rule_size * tree2->seed.num_rules) return;

  init_tree(offspring);
  /* Copy the structure from the first parent */
  offspring->seed.rule_size = tree1->seed.rule_size;
  offspring->seed.num_rules = tree1->seed.num_rules;
  free_rule_set(&offspring->seed);
  init_rule_set(&offspring->seed);

  /* crossover */
  cross_point = tree1->seed.rule_size * 
		(int)(tree1->seed.num_rules * (rand() / (float)RAND_MAX));
  /*  printf("Crossover at %i\n", cross_point); */
  memcpy(offspring->seed.rules, tree1->seed.rules, cross_point);
  memcpy(offspring->seed.rules + cross_point, tree2->seed.rules + cross_point,
				 length - cross_point);  
}
