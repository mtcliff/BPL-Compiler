/*parse.h
  Michael Clifford
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

struct node *program();
struct node *dec_list();
struct node *declaration();
struct node *type_spec();
struct node *params();
struct node *param_list();
struct node *param();
struct node *statement();
struct node *exp_stmt();
struct node *cmpd_stmt();
struct node *local_decs();
struct node *stmt_list();
struct node *if_stmt();
struct node *while_stmt();
struct node *return_stmt();
struct node *write_stmt();
struct node *expression();
struct node *comp_exp();
struct node *relop();
struct node *E();
struct node *addop();
struct node *T();
struct node *mulop();
struct node *F();
struct node *factor();
struct node *args();
struct node *arg_list();

#endif
