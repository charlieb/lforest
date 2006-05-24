#include "turtle.h"

struct turtle turtle_stack[256];
int angle_inc = 10;
int draw_length = 10;

void gen_branches(struct tree* tree)
{
  int branches = count_branches(tree);
  int t_stack_idx = 0;
  int i = -1;
  int line_ct = 0;

  if(branches > tree->n_branches) {
    if(tree->branches) free(tree->branches);
    tree->branches = malloc(branches * sizeof(struct line));
    if(tree->branches) 
      tree->n_branches = branches;
    else {
      printf("Failed to allocate memory for branches\n");
      exit(-1);
    }
      
  }

  /* Reset the turtle */
  turtle_stack[0].x = 0.0;
  turtle_stack[0].y = 0.0;

  /* Discover which of the !s are leaves */
  find_leaves(tree);

  for(i=0; i < tree->exp_size; ++i) {
    /* printf("(%f, %f) @ %i : %i : %c\n", 
	       turtle_stack[t_stack_idx].x, 
	       turtle_stack[t_stack_idx].y,
	       turtle_stack[t_stack_idx].angle,
	       tree->expansion[i],
	       tree->expansion[i]);
    */
    switch(tree->expansion[i]) {
    case DRAW_CHAR:
      tree->branches[line_ct].start.x = turtle_stack[t_stack_idx].x;
      tree->branches[line_ct].start.y = turtle_stack[t_stack_idx].y;
      walk(&turtle_stack[t_stack_idx]);
      tree->branches[line_ct].end.x = turtle_stack[t_stack_idx].x;
      tree->branches[line_ct].end.y = turtle_stack[t_stack_idx].y;
      line_ct++;
      break;
    case TURN_CL:
      turtle_stack[t_stack_idx].angle += angle_inc;
      if(turtle_stack[t_stack_idx].angle >= 360)
	turtle_stack[t_stack_idx].angle = 0;
      break;
    case TURN_ACL:
      turtle_stack[t_stack_idx].angle -= angle_inc;
      if(turtle_stack[t_stack_idx].angle < 0)
	turtle_stack[t_stack_idx].angle = 360 - angle_inc;
      break;
    case PUSH_CHAR:
      if(t_stack_idx == 255) break;
      turtle_stack[t_stack_idx+1].angle = turtle_stack[t_stack_idx].angle;
      turtle_stack[t_stack_idx+1].x = turtle_stack[t_stack_idx].x;
      turtle_stack[t_stack_idx+1].y = turtle_stack[t_stack_idx].y;
      t_stack_idx++;
      break;
    case POP_CHAR:
      if(t_stack_idx == 0) break;
      t_stack_idx--;
      break;
    default:
      /* Do nothing */
      break;
    }
  }
}

int count_branches(struct tree* tree)
{
  int i = -1;
  int branches = 0;
  for(i=0; i < tree->exp_size; ++i)
    if(tree->expansion[i] == DRAW_CHAR) 
      branches++;
  /* printf("count_branches found %i branches\n", branches); */
  return branches;
}

/* Leaf counting and descrimination */
#define LEAF_BLOCK_SIZE 256

/* Leaves can be defined as the last draw command before a pop 
   on the same level:
   (1()) - 1 = leaf
   (1(2)) - 2 = leaf
   ((1)(2)) - 1+2 = leaves
   (1() - 1 NOT a leaf
*/
void find_leaves(struct tree *tree)
{
  int i = 0;
  int draws = 0;
  int last_draw = -1;
  int last_draw_depth = 0;
  int paren_depth = 0;
  int leaf_no = 0;

  for(i=0; i < tree->exp_size; ++i)
    switch(tree->expansion[i])
      {
      case DRAW_CHAR: 
	last_draw_depth = paren_depth;
	last_draw = draws++;
	break;
      case PUSH_CHAR:
	paren_depth++;
	break;
      case POP_CHAR:
	/* ignore extra POPs */
	if(paren_depth > 0) {
	  if(last_draw != -1 && last_draw_depth == paren_depth) {
	    printf("Found line[%i] is leaf[%i] at char[%i]\n", 
		   last_draw, leaf_no, i);
	    if(leaf_no >= tree->n_leaves) {
	      tree->n_leaves += LEAF_BLOCK_SIZE;
	      tree->leaves = realloc(tree->leaves, 
				     tree->n_leaves * sizeof(int));
	    }
	    /* and add the leaf we found to the list */
	    tree->leaves[leaf_no++] = last_draw;
	    last_draw = -1;
	  }
	  paren_depth--;
	}
	break;
      }

  /* The last draw is always a leaf */
  if(last_draw != -1) {
    printf("Found line[%i] is leaf[%i] at char[%i]\n", 
	   last_draw, leaf_no, i);
    if(leaf_no >= tree->n_leaves) {
      tree->n_leaves += LEAF_BLOCK_SIZE;
      tree->leaves = realloc(tree->leaves, 
			     tree->n_leaves * sizeof(int));
    }
    /* and add the leaf we found to the list */
    tree->leaves[leaf_no++] = last_draw;
  }

  /* Set the correct number of leaves and free unused memory */
  tree->n_leaves = leaf_no;
  tree->leaves = realloc(tree->leaves, tree->n_leaves * sizeof(int));
}

void print_leaves(struct tree* tree)
{
  int i;
  for(i=0; i < tree->n_leaves; ++i) {
    printf("(%1.3f, %1.3f)->(%1.3f, %1.3f)\n", 
	   tree->branches[tree->leaves[i]].start.x, 
	   tree->branches[tree->leaves[i]].start.y,
	   tree->branches[tree->leaves[i]].end.x,
	   tree->branches[tree->leaves[i]].end.y);
  }
}

void print_branches(struct tree* tree)
{
  int i;
  for(i=0; i < tree->n_branches; ++i) {
    printf("(%1.3f, %1.3f)->(%1.3f, %1.3f)\n", 
	   tree->branches[i].start.x, tree->branches[i].start.y,
	   tree->branches[i].end.x, tree->branches[i].end.y);
  }
}
/*  ---- turtle commands ----  */

void walk(struct turtle* turt)
{
  /*  printf("Walk (%i, %i)\n", dx(turt->angle), dy(turt->angle));*/
  turt->x += draw_length * sin_cache(turt->angle);
  turt->y += draw_length * cos_cache(turt->angle); 
}

