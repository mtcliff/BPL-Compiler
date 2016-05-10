/*code_gen_H
  Michael Clifford
  Declare functions used in code generation*/

#include <stdio.h>

#ifndef CODE_GEN_H
#define CODE_GEN_H 

int findDepthDec(struct node *t, int d, int p);
int findDepthStmt(struct node *s, int d, int p);

void genCode(struct node *t);
void genCodeFun(struct node *f);
void genCodeStmt(struct node *s, int all);
void genCodeExp(struct node *e);

struct str {
  char *value;
  int label;
  struct str *next;
};

struct str *make_str(char *val);
void str_table(struct node *t);
void add_str(char *str);
int lookup_str(char *str);
void print_table(FILE *file);
struct str *table;  //string table head
int str_label;      //#for current string label
void free_table();

char *fp;     //frame pointer
char *sp;     //stack pointer
char *eax;    //%eax
char *rax;    //%rax
char *rsi;
char *esi;
char *rdi;
FILE *code;   //file containing assembly code

int getOffset(struct node *e);
void eval_args(struct node *a);  //evaluate f'n call args and push onto stack in reverse order

#endif
