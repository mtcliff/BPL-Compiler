/*code_gen.c
  Michael Clifford
  Generate assembly code from parse tree*/

#include "tree.h"
#include "code_gen.h"
#include "code_help.h"

int findDepthDec(struct node *t, int d, int p) {
  int max_p = 0;
  char *ptr;
  long len;
  if (t == NULL) {
    max_p = p;
  }
  else if (t -> v_or_f == 2) { //f'n dec
    findDepthDec(t -> params, 1, 0);
    t -> dec -> to_all = findDepthStmt(t -> child2, 2, 0);
    t -> dec -> to_all = t -> dec-> to_all * 8; 
    findDepthDec(t -> next, 0, 0);
  }
  else if (t -> v_or_f == 1) {
    if (t -> child1 -> kind == 35) { //array
      len = strtol(t->child1->arr_len, &ptr, 10);
      t -> dec -> depth = d;
      p = p + (len - 1);
      t -> dec -> pos = p;
      //      printf("Arr %s assigned depth %i and pos %i\n", t->child1->value, d, p);
    }
    else {
      t -> dec -> depth = d;
      t -> dec -> pos = p;
      //      printf("var %s assigned depth %i and pos %i\n", t->child1->value, d, p);
    }
    max_p = findDepthDec(t -> next, d, p+1);
  }
  else if (t -> kind == 9) { //parameter
    t -> dec -> depth = d;
    t -> dec -> pos = p;
    //    printf("param %s assigned depth %i and pos %i\n", t->child1->value, d, p);
    findDepthDec(t -> next, d, p+1);
  }
  return max_p;
}

int findDepthStmt(struct node *s, int d, int p) {
  int count = 0;
  int x;
  int y;
  int z;
  if (s == NULL) {
  } 
  else if (s -> kind == 10) { //compound stmt
    x = findDepthDec(s -> child1, d, p);
    y = findDepthStmt(s -> child2, d+1, x);
    z = findDepthStmt(s -> next, d, p);
    if (x+y > z) {     //find max pos
      if (y > 0) {
	count = y;
      }
      else {
	count = x;
      }
    }
    else {
      count = z;
    }
  }
  else if (s -> kind == 15) { //if stmt
    x = findDepthStmt(s -> child2, d, p);
    y = findDepthStmt(s -> child3, d, p); 
    z = findDepthStmt(s -> next, d, p);
    if (x > y && x > z) {     //find max pos
      count = x;
    }
    else if (y > x && y > z) {
      count = y;
    }
    else {
      count = z;
    }
  }
  else if (s -> kind == 16) { //while stmt
    x = findDepthStmt(s -> child2, d, p);
    y = findDepthStmt(s -> next, d, p);
    if (x > y) {
      count = x;
    }
    else {
      count = y;
    }
  }    
  else {
    count = findDepthStmt(s -> next, d, p);
  }
  return count;
} 

void str_table(struct node *t) {
  if (t == NULL) {
  }
  else if (t -> kind == 34) {
    add_str(t -> value);
  }
  else {
    str_table(t -> child1);
    str_table(t -> child2);
    str_table(t -> child3);
    str_table(t -> next);
  }
}

struct str *make_str(char *val) {
  struct str *s;
  s = malloc(sizeof(struct str));
  s -> label = str_label;
  s -> value = val;
  s -> next = NULL;
  str_label++;
  return s;
}

void add_str(char *str) {
  struct str *tab_curr;
  int i = 0;
  if (table == NULL) {
    table = make_str(str);
  }
  else {
    tab_curr = table;
    while (tab_curr != NULL) {
      if (strcmp(str, tab_curr->value) == 0) { //found string
	break;
      }
      else if (tab_curr -> next == NULL) {
	i = 1;
	break;
      }
      else {
	tab_curr = tab_curr -> next;
      }
    }
    if (i == 1) { //string not in table
      tab_curr -> next = make_str(str);
    }
  }
}

int lookup_str(char *str) {  //find string in table
  struct str *curr;
  curr = table;
  while (curr != NULL) {
    if (strcmp(str, curr->value) == 0) {
      break;
    }
    else {
      curr = curr -> next;
    }
  }
  return curr->label;
}

void print_table(FILE *file) {
  struct str *curr;
  char s[50];
  char *l;
  curr = table;
  while (curr != NULL) {
    l = string_lab(curr->label);
    sprintf(s, "%s: .string %s\n", l, curr->value);
    fputs(s, file);
    curr = curr -> next;
  }
}

void free_table() {
  struct str *t;
  struct str *u;
  t = table;
  if (t != NULL) {
    u = t -> next;
    free(t);
    while (u != NULL) {
      t = u;
      u = t -> next;
      free(t);
    }
  }
}

void genCode(struct node *t) {
  struct node *first;
  fp = "%rbx";
  sp = "%rsp";
  eax = "%eax";
  rax = "%rax";
  rsi = "%rsi";
  esi = "%esi";
  rdi = "%rdi";
  label_count = 0;
  code = fopen("code.s", "w");
  str_label = 0;
  table = NULL;
  first = t;
  while (t != NULL) {              //loop through all decs
    if (t -> v_or_f == 1) {        //generate global var labels
      if (t->child1->kind == 35) { //array
	glob_arr(code, t->child1->value, t->child1->arr_len);
      }
      else {
	glob_var(code, t->child1->value);
      }
    }
    t = t -> next;
  }
  t = first;
  header1(code);
  str_table(t);
  print_table(code);
  header2(code);
  while (t != NULL) {              //loop through all f'n decs
    if (t -> v_or_f == 2) {
      genCodeFun(t);
    }
    t = t -> next;
  }
  free_table();
}

void genCodeFun(struct node *f) {
  char *label;
  label = strcat(f->child1->value, ":\n");           //print f'n label
  fputs(label, code);
  std_line(code, "movq", sp, fp, "setup frame pointer");
  std_num_line(code, "subq", f->dec->to_all, sp, "allocate local var");
  genCodeStmt(f->child2, f->dec->to_all);
  std_num_line(code, "addq", f->dec->to_all, sp, "de-allocate local vars");
  no_line(code, "ret", "return from function call");
  fputs("\n", code);
}

void genCodeStmt(struct node *s, int all) {
  struct node *stmt;
  char *l1;
  char *l2;
  if (s == NULL) {
  }
  else if (s -> kind == 10) { //cmpd stmt
    stmt = s -> child2;
    while (stmt != NULL) {  //loop through statement list
      genCodeStmt(stmt, all);
      stmt = stmt -> next;
    }
  }
  else if (s -> kind == 14) { //exp stmt
    genCodeExp(s -> child1);
  }
  else if (s -> kind == 15) { //if
    l1 = label();
    l2 = label();
    genCodeExp(s->child1); //condition exp
    std_num_line(code, "cmpl", 0, eax, "check if stmt condition");
    one_line(code, "je", l1, "jump if condition false");
    genCodeStmt(s->child2, all); //if body
    one_line(code, "jmp", l2, "jump over else statement");
    print_label(code, l1);
    genCodeStmt(s->child3, all); //else body
    print_label(code, l2);
    free(l1);
    free(l2);
  }
  else if (s -> kind == 16) { //while
    l1 = label();
    l2 = label();
    print_label(code, l1);
    genCodeExp(s->child1);  //condition exp
    std_num_line(code, "cmpl", 0, eax, "check while loop condition");
    one_line(code, "je", l2, "jump if condition false");
    genCodeStmt(s->child2, all); //body
    one_line(code, "jmp", l1, "jump back to beginning of while loop");
    print_label(code, l2);
    free(l1);
    free(l2);
  }
  else if (s -> kind == 17) { //return
    genCodeExp(s->child1);
    std_num_line(code, "addq", all, sp, "deallocate local vars");
    no_line(code, "ret", "return from function");
  }
  else {  //write
    if (s -> child1 != NULL) {
      genCodeExp(s -> child1);
      if (strcmp(s->child1->type, "int") == 0) {
	std_line(code, "movl", eax, esi, "move value to print into %esi");
	std_dollar(code, "movq", ".WriteIntStr", rdi, "printf string = arg1");
      }      
      else {
	std_line(code, "movq", rax, rsi, "move value to print into rsi");
	std_dollar(code, "movq", ".WriteStrStr", rdi, "printf string = arg1");
      }
      std_num_line(code, "movl", 0, eax, "clear return value");
      one_line(code, "call", "printf", "print");
    }
    else {
      std_dollar(code, "movq", ".WritelnStr", rdi, "printf string = arg1");
      std_num_line(code, "movl", 0, eax, "clear return value");
      one_line(code, "call", "printf", "print");
    }
  }
}

void genCodeExp(struct node *e) {
  int offset;
  struct node *f;
  struct node *a;
  int arg_count;
  int sl;
  char *l1;
  char *l2;
  char *lab;
  char num[10];
  if (e == NULL) {
  } 
  else if (e -> kind == 21) { //comp exp
    genCodeExp(e -> child1);
    if (e -> child2 != NULL) { //comparison
      one_line(code, "push", rax, "push left side of comparison onto stack");
      genCodeExp(e -> child3);
      off_dst(code, "cmpl", eax, 0, sp, "comparison");
      l1 = label();
      l2 = label();
      if (strcmp(e->child2->value, "<=") == 0) {
	one_line(code, "jle", l1, "jump if <=");
      }
      else if (strcmp(e->child2->value, "<") == 0) {
	one_line(code, "jl", l1, "jump if <");
      }
      else if (strcmp(e->child2->value, "==") == 0) {
	one_line(code, "je", l1, "jump if ==");
      }
      else if (strcmp(e->child2->value, "!=") == 0) {
	one_line(code, "jne", l1, "jump if !=");
      }
      else if (strcmp(e->child2->value, ">") == 0) {
	one_line(code, "jg", l1, "jump if >");
      }
      else { //>=
	one_line(code, "jge", l1, "jump if >=");
      }
      std_num_line(code, "movl", 0, eax, "move 0 to accumulator (if false)");
      one_line(code, "jmp", l2, "jump if false");
      print_label(code, l1);
      std_num_line(code, "movl", 1, eax, "move 1 to accumulator (if true)");
      print_label(code, l2);
      std_num_line(code, "addq", 8, sp, "pop left side of comparison from stack");
      free(l1);
      free(l2);
    }
  }
  
  else if (e -> kind == 38) { //var assign
    f = e -> child1;
    if (f -> kind == 36) { //factor pre (pointer)
      if (f -> child1 -> dec -> depth == 0) { //global var
	std_dollar(code, "movq", f->child1->value, rax, "move global var to accumulator");
      }
      else {
	offset = getOffset(f);
	std_line(code, "movq", fp, rax, "move frame pointer to accumulator");
	if (f->child1->dec->depth == 1) {
	  std_num_line(code, "add", offset, rax, "param address");
	}
	else {
	  offset = offset * -1;
	  std_num_line(code, "sub", offset, rax, "local address");
	}
      }
      off_src(code, "movq", 0, rax, rax, "dereference");
      one_line(code, "push", rax, "save l-value on stack");
      genCodeExp(e -> child2);
      off_src(code, "movq", 0, sp, rsi, "put l-value into %rsi");
      off_dst(code, "movq", rax, 0, rsi, "assignment");
      std_num_line(code, "addq", 8, sp, "pop l-value from stack");
    } 
    else if (f -> child1 -> kind == 32) { //id
      offset = getOffset(f);
      if (offset > 0 || offset < -1) { //param or local var
	std_line(code, "movq", fp, rax, "put frame pointer into accumulator");
	if (offset > 0) {
	  std_num_line(code, "add", offset, rax, "param address");
	}
	else {
	  offset = offset * -1;
	  std_num_line(code, "sub", offset, rax, "local address");
	}
	one_line(code, "push", rax, "save l-value on stack");
	genCodeExp(e -> child2);
	off_src(code, "movq", 0, sp, rsi, "put l-value into %rsi");
	off_dst(code, "movq", rax, 0, rsi, "assignment");
	std_num_line(code, "addq", 8, sp, "pop l-value from stack");
      }
      else { //global var
	std_dollar(code, "movq", f->child1->value, rax, "put global var into accumulator");
        one_line(code, "push", rax, "save l-value on stack");
	genCodeExp(e -> child2);
	off_src(code, "movq", 0, sp, rsi, "put l-value into %rsi");
	off_dst(code, "movq", rax, 0, rsi, "assignment");
	std_num_line(code, "addq", 8, sp, "pop l-value from stack");
      }
    }
    else if (f -> child1 -> kind == 35) { //array
      offset = getOffset(f);
      if (offset > 0 || offset < -1) { //param or local var
	std_line(code, "movq", fp, rax, "put frame pointer into accumulator");
	if (offset > 0) {
	  std_num_line(code, "add", offset, rax, "array param address");
	}
	else { 
	  offset = offset * -1;
	  std_num_line(code, "sub", offset, rax, "local array address");
	}
      }
      else { // global var
	std_dollar(code, "movq", f->child1->value, rax, "put start value of array into accumulator");
      }
      one_line(code, "push", rax, "save value on stack");
      genCodeExp(f->child1->arr_index);
      std_num_line(code, "imul", 8, eax, "calculate array index");
      off_src(code, "movq", 0, sp, rsi, "put array start index into %rsi");
      std_num_line(code, "addq", 8, sp, "pop stack");
      std_line(code, "addq", rax, rsi, "compute address of array index");
      std_line(code, "movq", rsi, rax, "put array index into accumulator");
      one_line(code, "push", rax, "push array index onto stack");
      genCodeExp(e -> child2);
      off_src(code, "movq", 0, sp, rsi, "put l-value into %rsi");
      off_dst(code, "movq", rax, 0, rsi, "assignment");
      std_num_line(code, "addq", 8, sp, "pop l-value from stack");
    }
    //do for pointers
  }

  else if (e -> kind == 24) { //addop
    genCodeExp(e -> child1);
    one_line(code, "push", rax, "save left operand on stack");
    genCodeExp(e -> child2);
    if (strcmp(e->value, "+") == 0) {
      off_src(code, "addl", 0, sp, eax, "perform addition");
    }
    else {
      off_dst(code, "subl", eax, 0, sp, "perform subtraction");
      off_src(code, "movl", 0, sp, eax, "move result to accumulator");
    }
    std_num_line(code, "addq", 8, sp, "pop left operand from stack");
  }

  else if (e -> kind == 26) { //mulop
    if (strcmp(e -> value , "*") == 0) {
      genCodeExp(e -> child1);
      one_line(code, "push", rax, "save left operand on stack");
      genCodeExp(e -> child2);
      off_src(code, "imul", 0, sp, eax, "perform multiplication");
      std_num_line(code, "addq", 8, sp, "pop left operand from stack");
    }      
    else {  //division
      genCodeExp(e -> child1);
      one_line(code, "push", rax, "push dividend onto stack");
      genCodeExp(e -> child2);
      std_line(code, "movl", eax, "%ebp", "move divisor into %rbx");
      off_src(code, "movl", 0, sp, eax, "move dividend to accumulator");
      no_line(code, "cltq", "sign-extend upper half of rax");
      no_line(code, "cqto", "sign-extend rax into rdx");
      one_line(code, "idivl", "%ebp", "perform division");
      std_num_line(code, "addq", 8, sp, "pop dividend off stack");
    }
  }
  else if (e -> kind == 27) { //f node
    if (e -> child1 -> kind == 21) {
      genCodeExp(e->child1);
    }
    else if (e -> child1 -> kind == 32) { //id
      if (strcmp(e->type, "int array") == 0 || strcmp(e -> type, "string array") == 0) {
	if (e -> child1 -> dec -> depth == 0) { //global
	  std_dollar(code, "movq", e->child1->value, rax, "global array");
	}
	else {
	  offset = getOffset(e);
	  std_line(code, "movq", fp, rax, "move frame pointer to accumulator");
	  if (e->child1->dec->depth == 1) { //param
	    std_num_line(code, "add", offset, rax, "array param address");
	  }
	  else { //local array
	    offset = offset * -1;
	    std_num_line(code, "sub", offset, rax, "local array address");
	  }
	}
      }
      else if (e -> child1 -> dec -> depth == 0) { //global var
	if (strcmp(e->type, "int pointer") == 0 || strcmp(e->type, "string pointer") == 0) {
	std_line(code, "movq", e->child1->value, rax, "move global pointer to accumulator");
	}
	else {
	  std_line(code, "movq", e->child1->value, rax, "move global var to accumulator");
	}
      }
      else {
	offset = getOffset(e);
 	if (e->child1->dec->depth == 1) {
	  if (strcmp(e->type, "int pointer") == 0 || strcmp(e->type, "string pointer") == 0) {
	    off_src(code, "movq", offset, fp, rax, "move param to accumulator");
	  }
	  else {
	    off_src(code, "movl", offset, fp, eax, "move param to accumulator");
	  }
	}
	else {
	  if (strcmp(e->type, "int pointer") == 0 || strcmp(e->type, "string pointer") == 0) {
	    off_src(code, "movq", offset, fp, rax, "move local var to accumulator");
	  }
	  else {
	    off_src(code, "movl", offset, fp, eax, "move local var to accumulator");
	  }
	}
      }
    }
    else if (e -> child1 -> kind == 35) { //array
      if (e -> child1 -> dec -> depth == 0) { //global array
	std_dollar(code, "movq", e->child1->value, rax, "move global array to accumulator");
      }
      else {
	std_line(code, "movq", fp, rax, "put frame pointer into accumulator");
	offset = getOffset(e);
	if (e->child1->dec->depth == 1) {
	  off_src(code, "movq", offset, fp, rax, "array param address");
	}
	else {
	  offset = offset * -1;
	  std_num_line(code, "sub", offset, rax, "local array address");
	}
      }
      if (e->child1->arr_index != NULL) {
	one_line(code, "push", rax, "save start address of array on stack");
	genCodeExp(e->child1->arr_index);
	std_num_line(code, "imul", 8, eax, "calculate array index");
	off_src(code, "movq", 0, sp, rsi, "put array start index into %rsi");
	std_num_line(code, "addq", 8, sp, "pop stack");
	std_line(code, "addq", rax, rsi, "compute address of array index");
	off_src(code, "movl", 0, rsi, eax, "move array value into accumulator");
      }
    }
    else if (e -> child1 -> kind == 33) { //num
      std_dollar(code, "movl", e->child1->value, eax, "move int to accumulator");  
    }
    else if (e -> child1 -> kind == 34) { //string
      sl = lookup_str(e->child1->value);
      lab = string_lab(sl);
      std_dollar(code, "movq", lab, rax, "move string into accumulator");
      free(lab);
    }
    else if (e -> child1 -> kind == 29) { //f'n call
      eval_args(e -> child1 -> child2);
      one_line(code, "push", fp, "push frame pointer on to stack");
      one_line(code, "call", e ->child1->child1->value, "call function");
      one_line(code, "pop", fp, "restore frame pointer");
      a = e -> child1 -> child2; //args
      if (a -> value == NULL) { //non-empty args
	arg_count = 0;
	while (a != NULL) {
	  arg_count += 1;
	  a = a -> next;
	}
	arg_count = arg_count * 8;
	std_num_line(code, "addq", arg_count, sp, "pop args from stack"); 
      }
    }
    else { //read call
      std_num_line(code, "subq", 40, sp, "decrement stack pointer by 40");
      std_line(code, "movq", sp, rax, "move stack pointer to accumulator");
      std_num_line(code, "add", 24, rax, "address 24 below new stack pointer");
      std_line(code, "movq", rax, rsi, "move 24 below stack pointer to %rsi");
      std_dollar(code, "movq", ".ReadIntStr", rdi, "prepare for read() call");
      one_line(code, "call", "scanf", "execute read");
      off_src(code, "movl", 24, sp, eax, "put result in accumulator");
      std_num_line(code, "addq", 40, sp, "increment stack pointer");
    }
  }
  else {  //factor pre
    if (strcmp(e->value, "-") == 0) {
      sprintf(num, "-%s", e->child1->value); 
      std_dollar(code, "movl", num, eax, "move int to accumulator");
    }
    else if (strcmp(e->value, "*") == 0) {
      if (e -> child1 -> dec -> depth == 0) { //global var
	std_dollar(code, "movq", e->child1->value, rax, "move global var to accumulator");
      }
      else {
	offset = getOffset(e);
	if (e->child1->dec->depth == 1) {
	  off_src(code, "movq", offset, fp, rax, "move param to accumulator");
	}
	else {
	  off_src(code, "movq", offset, fp, rax, "move local var to accumulator");
	}
      }
      off_src(code, "movq", 0, rax, rax, "dereference");
    }
    else { //&
      if (e -> child1 -> kind == 32) { //id
	if (e -> child1 -> dec -> depth == 0) { //global var
	  std_dollar(code, "movq", e->child1->value, rax, "move global var to accumulator");
	}
	else {
	  offset = getOffset(e);
	  std_line(code, "movq", fp, rax, "move frame pointer to accumulator");
	  if (e->child1->dec->depth == 1) {
	    std_num_line(code, "add", offset, rax, "param address");
	  }
	  else {
	    offset = offset * -1;
	    std_num_line(code, "sub", offset, rax, "local address");
	  }
	}
      }
      else if (e -> child1 -> kind == 35) { //array
	offset = getOffset(e);
	if (offset > 0 || offset < -1) { //param or local var
	  std_line(code, "movq", fp, rax, "put frame pointer into accumulator");
	  if (offset > 0) {
	    std_num_line(code, "add", offset, rax, "array param address");
	  }
	  else { 
	    offset = offset * -1;
	    std_num_line(code, "sub", offset, rax, "local array address");
	  }
	}
	else { // global var
	  std_dollar(code, "movq", e->child1->value, rax, "put start value of array into accumulator");
	}
	one_line(code, "push", rax, "save value on stack");
	genCodeExp(e->child1->arr_index);
	std_num_line(code, "imul", 8, eax, "calculate array index");
	off_src(code, "movq", 0, sp, rsi, "put array start index into %rsi");
	std_num_line(code, "addq", 8, sp, "pop stack");
	std_line(code, "addq", rax, rsi, "compute address of array index");
	std_line(code, "movq", rsi, rax, "put array index into accumulator");
      }
    } 
  } 
}

int getOffset(struct node *e) {
  int offset = 0;
  if (e -> child1 -> kind == 32 || e -> child1 -> kind == 35) { //id, array
    if (e -> child1 -> dec -> depth == 0) { //global var
      offset = -1;
    }
    else if (e -> child1 -> dec -> depth == 1) { //param
      offset = 16+8*e->child1->dec->pos;
    }
    else { //local var
      offset = -8-8*e->child1->dec->pos;
    }
  } 
  return offset;
}

void eval_args(struct node *a) {
  if (a -> value != NULL) { //empty args
  }
  else if (a -> next == NULL) { //last arg in list
    genCodeExp(a);
    one_line(code, "push", rax, "push arg onto stack");
  }
  else {
    eval_args(a -> next);
    genCodeExp(a);
    one_line(code, "push", rax, "push arg onto stack");
  }
}
