/*tree.h
  Michael Clifford
  declare node for parser tree and relevant functions*/

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  int kind;
  int line;
  int v_or_f;
  int is_var;
  char *arr_len;
  char *name;
  char *type;
  char *value;
  struct node *params;
  struct node *arr_index;
  struct node *next;
  struct node *child1;
  struct node *child2;
  struct node *child3;
  struct symbol *dec;
};

struct symbol {
  struct symbol *next;
  int v_or_f;
  int line;
  char *name;
  char *type;
  struct node *params;
  int depth;
  int pos;
  int to_all;
};

struct node *tree;

int debug;

struct node *make_node(int lnum, int k, char *nm);
void print_tree(struct node *n);
struct symbol *make_symbol(struct node *n, char which);

struct symbol *glob_head;
struct symbol *loc_head;
struct symbol *global;
struct symbol *local;

void print_glob();
void print_loc();

void find_dec(struct node *n);

void print_exp(char *type, int line, int kind);
void print_fact(char *type, int line, int kind, char *value);
void print_pre(char *type, int line, char *val, char *value);

char *param_type(struct node *p);

void free_tree(struct node *t);
void free_symbol(struct symbol *s);
void free_glob();

#endif

