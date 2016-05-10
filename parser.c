/*parser.c
  Michael Clifford
  Implement LL parse tree*/

#include "token.h"
#include "tree.h"
#include "parser.h"

struct node *program() {
  struct node *dec_list();
  struct node *prog_node = make_node(l, 1, "PROG_NODE");
  prog_node -> child1 = dec_list();
  return prog_node;
}

struct node *dec_list() {
  struct node *declaration();
  struct node *dec_node;
  struct node *dn = NULL;
  if (currtok -> kind != 0) { //!=EOF
    dec_node = declaration();
    dn = dec_node;
  }
  while (currtok -> kind != 0) {

    dec_node -> next = declaration();
    dec_node = dec_node -> next;
  }
  dec_node = dn;
  return dec_node;
}

struct node *declaration() {
  struct node *type_spec();
  struct node *cmpd_stmt();
  struct node *params();
  struct node *dec_node = make_node(l, 3, "DEC_NODE");
  dec_node -> type = type_spec() -> value;
  if (currtok -> kind == 27) { //=='*'
    dec_node -> v_or_f = 1;
    currtok = getNextToken();
    dec_node -> child1 = make_node(l, 39, "POINT_ID");
    if (currtok -> kind == 32) {
      dec_node -> child1 -> value = currtok -> value;
      currtok = getNextToken();
      if (currtok -> kind == 13) {
	printf("Parse Error: Can't have array pointer (line %i)\n", l);
	exit(1);
      }
    }
    else {
      printf("Parse_Error: Expecting identifier at line %i\n", l);
      exit(1);
    }
  }
  else {
    if (currtok -> kind == 32) {
      dec_node -> child1 = make_node(l, 32, "ID");
      dec_node -> child1 -> value = currtok -> value;      
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting identifier at line %i\n", l);
      exit(1);
    }
    if (currtok -> kind == 13) { //=='['
      dec_node -> v_or_f = 1;
      dec_node -> child1 -> name = "ARR";
      dec_node-> child1 -> kind = 35;
      currtok = getNextToken();
      if (currtok -> kind == 33) {
	dec_node -> child1 -> arr_len = currtok -> value;
	currtok = getNextToken();
      }
      else {
	printf("Parse_Error: Expecting number at line %i\n", l);
	exit(1);
      }
      if (currtok -> kind == 14) { //==']'
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting closing ']' at line %i\n", l);
	exit(1);
      }
    }
    else if (currtok -> kind == 17) { //=='(' - check to see if f'n dec
      dec_node -> v_or_f = 2;
      currtok = getNextToken();
      dec_node -> params = params();
      if (currtok -> kind == 18) { //==')'
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting closing ')' at line %i\n", l);
	exit(1);
      }
      if (currtok -> kind == 15) { //=='{'
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting '{' at line %i\n", l);
	exit(1);
      }
      dec_node -> child2 = cmpd_stmt();
    }
    else {
      dec_node -> v_or_f = 1;
    }
  }
  if (dec_node -> v_or_f == 1) {
    if (currtok -> kind == 11) { //==';'
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting ';' at line %i\n", l);
      exit(1);
    }
  }
  return dec_node;
}
 
struct node *type_spec() {
  struct node *type_node = make_node(l, 5, "TYPE_NODE");
  if (currtok->kind >= 1 && currtok->kind <=3) {
    type_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else {
    printf("Parse Error: Invalid type at line %i\n", l);
    exit(1);
  }
  return type_node;
}

struct node *params() {
  struct node *param_list();
  struct node *params_node = make_node(l, 7, "PARAM_NODE");
  if (currtok->kind == 18) { //==')'
    params_node -> value = "void";
  }
  else {
    params_node = param_list();
  }
  return params_node;
}

struct node *param_list() {
  struct node *param();
  struct node *param_node;
  struct node *pn;
  int ll = l;
  if (currtok -> kind != 18) { //==')'
    param_node = param();
    pn = param_node;
  }
  while (currtok -> kind != 18) {
    if (currtok -> kind == 0) {
      printf("Parse Error: Expecting closing ')' at line %i\n", ll);
      exit(1);
    }
    if (currtok -> kind == 12) { //','
      currtok = getNextToken();
      if (currtok -> kind == 18) {
	printf("Parse Error: Uneeded comma in param list at line %i\n", l);
	exit(1);
      }
    }
    else {
      printf("Parse Error: Expecting ',' in param list at line %i\n", l);
      exit(1);
    }
    param_node -> next = param();
    param_node = param_node -> next;
  }
  param_node = pn;
  return param_node;
}

struct node *param() {
  struct node *type_spec();
  struct node *param_node = make_node(l, 9, "PARAM_NODE");
  param_node -> type = type_spec() -> value;
  if (currtok -> kind == 27) { //=='*'
    currtok = getNextToken();
    param_node -> child1 = make_node(l, 39, "POINT_ID");
    if (currtok -> kind == 32) {
      param_node -> child1 -> value = currtok -> value;
      currtok = getNextToken();
    }
    else {
      printf("Parse_Error: Expecting identifier at line %i\n", l);
      exit(1);
    }
  }
  else {
    if (currtok -> kind == 32) {
      param_node -> child1 = make_node(l, 32, "ID");
      param_node -> child1 -> value = currtok -> value;      
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting identifier at line %i\n", l);
      exit(1);
    }
    if (currtok -> kind == 13) { //=='['
      currtok = getNextToken();
      param_node -> child1 -> name = "ARR";
      param_node -> child1 -> kind = 35;
      if (currtok -> kind == 14) { //==']'
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting closing ']' at line %i\n", l);
	exit(1);
      }
    }
  }
  return param_node;
}

struct node *statement() {
  struct node *exp_stmt();
  struct node *cmpd_stmt();
  struct node *if_stmt();
  struct node *while_stmt();
  struct node *return_stmt();
  struct node *write_stmt();
  struct node *stmt_node = make_node(l, 13, "STMT_NODE");
  if (currtok->kind == 15) { //is '{' => compound statement
    currtok = getNextToken();
    stmt_node = cmpd_stmt();
  }
  else if (currtok->kind == 4) { //if
    currtok = getNextToken();
    stmt_node = if_stmt();
  }
  else if (currtok->kind == 6) { //while
    currtok = getNextToken();
    stmt_node = while_stmt(); 
  } 
  else if (currtok->kind == 7) { //return
    currtok = getNextToken();
    stmt_node = return_stmt();
  }
  else if (currtok->kind == 8 || currtok->kind == 9) { //write
    stmt_node = write_stmt();
  }
  else {
    stmt_node = exp_stmt();
  }
  return stmt_node;
}

struct node *exp_stmt() {
  struct node *expression();
  struct node *expst_node = make_node(l, 14, "EXPST_NODE");
  if (currtok->kind == 11) {
    currtok = getNextToken();
  }
  else {
    expst_node -> child1 = expression(); 
    if (currtok->kind == 11) { //== ';'
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting ';' at line %i\n", l);
      exit(1);
    }
  }
  return expst_node;
}

struct node *cmpd_stmt() {
  struct node *local_decs();
  struct node *stmt_list();
  struct node *cmpd_node = make_node(l, 10, "CMPD_STMT_NODE");
  cmpd_node -> child1 = local_decs();
  cmpd_node -> child2 = stmt_list();
  if (currtok->kind == 16) { //is'}'
    currtok = getNextToken();
  }
  else {
    printf("Parse Error: Expecting '}' at line %i\n", l);
    exit(1);
  }
  return cmpd_node;
}

struct node *local_decs() {
  struct node *declaration();
  struct node *dec_node;
  struct node *dn = NULL;
  if (currtok->kind == 1 || currtok->kind ==2 || currtok->kind ==3) { //int/void/string
    dec_node = declaration();
    if (dec_node -> v_or_f == 2) {
      printf("Parse Error: Function declared inside function at line %i\n", l);
      exit(1);
    }
    dn = dec_node;
  }
  while (currtok->kind == 1 || currtok->kind == 2 || currtok->kind == 3) {
    dec_node -> next = declaration();
    if (dec_node -> v_or_f == 2) {
      printf("Parse Error: Function declared inside function at line %i\n", l);
      exit(1);
    }
    dec_node = dec_node -> next;
  }
  dec_node = dn;
  return dec_node;
}

struct node *stmt_list() {
  struct node *statement();
  struct node *stmt_node;
  struct node *sn = NULL;
  int ll = l;
  if (currtok -> kind != 16) {
    stmt_node = statement();
    sn = stmt_node;
  }
  while (currtok -> kind != 16) {
    if (currtok -> kind == 0) {
      printf("Parse Error: Expecting closing '}' at line %i\n", ll);
      exit(1);
    }
    stmt_node -> next = statement();
    stmt_node = stmt_node -> next;
  }
  stmt_node = sn;
  return stmt_node;
}

struct node *if_stmt() {
  struct node *expression();
  struct node *statement();
  struct node *if_node = make_node(l, 15, "IF_STMT_NODE");
  if (currtok->kind == 17) { //=='('
    currtok = getNextToken();
    if_node -> child1 = expression();
  }
  else {
    printf("Parse Error: Expecting '(' at line %i\n", l);
    exit(1);
  }
  if (currtok -> kind == 18) { //==')'
    currtok = getNextToken();
    if_node -> child2 = statement();
  }
  else {
    printf("Parse Error: Expecting ')' at line %i\n", l);
    exit(1);
  }
  if (currtok -> kind == 5) { //else
    currtok = getNextToken();
    if_node -> child3 = statement();
  }
  return if_node;
}

struct node *while_stmt() {
  struct node *expression();
  struct node *statement();
  struct node *while_node = make_node(l, 16, "WHILE_STMT_NODE");
  if (currtok->kind == 17) { //'('
    currtok = getNextToken();
    while_node -> child1 = expression();
    if (currtok->kind == 18) {//')'
      currtok = getNextToken();
      while_node -> child2 = statement();
    }
    else {
      printf("Parse Error: Expecting ')' in while statement on line %i\n", l);
      exit(1);
    }
  }
  else {
    printf("Parse Error: Expexting '(' in while statement on line %i\n", l);
    exit(1);
  }
  return while_node;
}

 struct node *return_stmt() {
   struct node *expression();
   struct node *ret_node = make_node(l, 17, "RETURN_STMT_NODE");
   if (currtok->kind == 11) { //==';'
     currtok = getNextToken();
   }
   else {
     ret_node -> child1 = expression();
     if (currtok->kind == 11) {
       currtok = getNextToken();
     }
     else {
       printf("Parse Error: Expecting ';' in return statement at line %i\n", l);
       exit(1);
     }
   }
   return ret_node;
 }
 
struct node *write_stmt() {
  struct node *expression();
  struct node *write_node = make_node(l, 18, "WRITE_STMT_NODE");
  int isexp = 0;
  if (currtok -> kind == 8) { //write()
    isexp = 1;
  }
  currtok = getNextToken();
  if (currtok->kind == 17) { //=='('
    currtok = getNextToken();
    if (isexp == 1) {
      write_node -> child1 = expression();
    }
    if (currtok->kind == 18) { //==')'
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting closing ')' at line %i\n", l);
      exit(1);
    }
    if (currtok-> kind == 11) { //==';'
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting ';' at line %i\n", l);
      exit(1);
    }
  }
  else {
    printf("Parse Error: Expecting '(' at line %i\n", l);
  }
  return write_node;
}

struct node *expression() {
  struct node *comp_exp();
  return comp_exp();
}

struct node *comp_exp() {
  struct node *expression();
  struct node *E();
  struct node *relop();
  struct node *cmpex_node = make_node(l, 21, "EXP_NODE");
  cmpex_node -> child1 = E();
  struct node *place;
  if (currtok -> kind == 30) { //'='
    if (cmpex_node -> child1 -> is_var == 0) {
      printf("Parse Error: Non-var on left side of assignment at line %i\n", l);
      exit(1);
    }
    place = cmpex_node -> child1;
    cmpex_node = make_node(l, 38, "VAR_ASSIGN");
    cmpex_node -> child1 = place;
    currtok = getNextToken();
    cmpex_node -> child2 = expression();
  }
  else if (currtok -> kind >= 19 && currtok -> kind <=24) { //';'
    cmpex_node -> child2 = relop();
    cmpex_node -> child3 = E();
  }
  return cmpex_node;
}

struct node *relop() {
  struct node *relop_node = make_node(l, 22, "RELOP");
  if (currtok -> kind >= 19 && currtok -> kind <= 24) {
    relop_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else {
    printf("%s", currtok -> value);
    printf("Parse Error: Expecting RELOP at line %i\n", l);
    exit(1);
  }
  return relop_node;
}

struct node *E() {
  struct node *T();
  struct node *addop();
  struct node *t_node = T();
  if (currtok -> kind == 30) { //'='
      return t_node;
  }
  struct node *t1;
  while (currtok -> kind == 25 || currtok -> kind == 26) { //is ADDOP
    t1 = addop();
    t1 -> child1 = t_node;
    t1 -> child2 = T();
    t_node = t1;
  }
  return t_node;
}

struct node *addop() {
  struct node *addop_node = make_node(l, 24, "ADDOP");
  if (currtok -> kind == 25 || currtok -> kind == 26) { //+ or -
    addop_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else {
    printf("Parse Error: Expecting ADDOP at line %i\n", l);
    exit(1);
  }
  return addop_node;
}

struct node *T() {
  struct node *F();
  struct node *mulop();
  struct node *f_node = F();
  struct node *f1;
  while (currtok -> kind >= 27 && currtok -> kind <= 29) { //is mulop
    f1 = mulop();
    f1 -> child1 = f_node;
    f1 -> child2 = F();
    f_node = f1;
  }
  return f_node;
}

struct node *mulop() {
  struct node *mulop_node = make_node(l, 26, "MULOP");
  if (currtok -> kind >= 27 && currtok -> kind <= 29) { //* or / or %
    mulop_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else {
    printf("Parse Error: Expecting MULOP at line %i\n", l);
    exit(1);
  }
  return mulop_node;
}

struct node *F() {
  struct node *factor();
  struct node *f_node = make_node(l, 27, "F_NODE");
  if (currtok -> kind == 26 || currtok -> kind == 27 || currtok -> kind == 31) { // -, &, or *
    f_node = make_node(l, 36, "FACTOR_PRE");
    f_node -> value = currtok -> value;
    currtok = getNextToken();
    f_node -> child1 = factor();
    if (f_node -> child1 -> is_var == 1) {
      f_node -> is_var = 1;
    }
  }
  else {
    f_node -> child1 = factor();
    if (f_node -> child1 -> is_var == 1) {
      f_node -> is_var = 1;
    }
  }
  return f_node;
}

struct node *factor() {
  struct node *expression();
  struct node *args();
  struct node *factor_node;// = make_node(l, 28, "FACTOR_NODE");
  struct node *place_node;
  if (currtok -> kind == 17) { //'('
    currtok = getNextToken();
    factor_node = expression();
    if (currtok -> kind == 18) { //')'
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting closing ')' at line %i\n", l);
      exit(1);
    }
  }
  else if (currtok -> kind == 10) { //read
    factor_node = make_node(l, 37, "READ_CALL");
    currtok = getNextToken();
    if (currtok -> kind == 17) { //'('
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting '(' at line %i\n", l);
      exit(1);
    }
    if (currtok -> kind == 18) {
      currtok = getNextToken();
    }
    else {
      printf("Parse Error: Expecting closing ')' at line %i\n", l);
      exit(1);
    }
  }
  else if (currtok -> kind == 33) { //num
    factor_node = make_node(l, 33, "NUM");
    factor_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else if (currtok -> kind == 34) { //string
    factor_node = make_node(l, 34, "STRING");
    factor_node -> value = currtok -> value;
    currtok = getNextToken();
  }
  else if (currtok -> kind == 27) { //=='*'
    currtok = getNextToken();
    factor_node = make_node(l, 34, "POINT_ID");
    factor_node -> is_var = 1;
    if (currtok -> kind == 32) {
      factor_node -> value = currtok -> value;
      currtok = getNextToken();
    }
    else {
      printf("Parse_Error: Expecting identifier at line %i\n", l);
      exit(1);
    }
  }
  else if (currtok -> kind == 32) { //identifier
    place_node = make_node(l, 32, "ID");
    place_node -> value = currtok -> value;      
    currtok = getNextToken();
    if (currtok -> kind == 17) { //'(' => f'n call
      factor_node = make_node(l, 29, "FUN_CALL_NODE");
      factor_node -> child1 = place_node;
      currtok = getNextToken();
      factor_node -> child2 = args();
      if (currtok -> kind == 18) {
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting closing ')' in function call at line %i\n", l);
	exit(1);
      }
    }
    else if (currtok -> kind == 13) { //=='['
      currtok = getNextToken();
      if (currtok -> kind == 14) {
	printf("Parse Error: Expecting array index at line %i\n", l);
	exit(1);
      }
      factor_node = place_node;
      factor_node -> name = "ARR";
      factor_node -> kind = 35;
      factor_node -> is_var = 1;
      factor_node -> arr_index = expression();
      if (currtok -> kind == 14) { //==']'
	currtok = getNextToken();
      }
      else {
	printf("Parse Error: Expecting closing ']' at line %i\n", l);
	exit(1);
      }
    }
    else {
      factor_node = place_node;
      factor_node -> is_var = 1;
    }
  }
  else {
    printf("Parse Error: Expecting factor at line %i\n", l);
    exit(1);
  }
  return factor_node;
}

struct node *args() {
  struct node *arg_list();
  struct node *args_node = make_node(l, 30, "ARGS_NODE");
  if (currtok -> kind == 18) { //')'
    args_node -> value = "<empty>";
  }
  else {
    args_node = arg_list();
  }
  return args_node;
}

struct node *arg_list() {
  struct node *expression();
  struct node *exp_node;
  struct node *en;
  int ll = l;
  if (currtok -> kind != 18) { //==')'
    exp_node = expression();
    en = exp_node;
  }
  while (currtok -> kind != 18) {
    if (currtok -> kind == 0) {
      printf("Parse Error: Expecting closing ')' at line %i\n", ll);
      exit(1);
    }
    if (currtok -> kind == 12) { //','
      currtok = getNextToken();
      if (currtok -> kind == 18) {
	printf("Parse Error: Uneeded comma in function call at line %i\n", l);
	exit(1);
      }
    }
    else {
      printf("Parse Error: Expecting ',' in function call  at line %i\n", l);
      exit(1);
    }
    exp_node -> next = expression();
    exp_node = exp_node -> next;
  }
  exp_node = en;
  return exp_node;
}
