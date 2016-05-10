/*type_checker.c
  Michael Clifford
  Type check my parse tree
*/

#include "token.h"
#include "tree.h"
#include "type_checker.h"

void findRef(struct node *t) {
  void findRefStmt(struct node *s);
  struct node *dec;
  dec = t -> child1;
  if (dec != NULL) {
    glob_head = make_symbol(dec, 'g');
    global = glob_head;
    if (dec -> v_or_f == 2) {
      if (dec -> params -> kind == 9) {
	findRefStmt(dec -> params);
      }
      findRefStmt(dec -> child2);
      loc_head = NULL;
    }
    dec = dec -> next;
    while (dec != NULL) {
      global -> next = make_symbol(dec, 'g');
      if (dec -> v_or_f == 2) { //function dec
	if (dec -> params -> kind == 9) {
	  findRefStmt(dec -> params); //params
	}
	findRefStmt(dec -> child2); //call on function body
	loc_head = NULL;
      }
      dec = dec -> next;
      global = global -> next;
    }
  }
}

void findRefStmt(struct node *s) {
  void findRefExp(struct node *e);
  struct node *c;
  struct symbol *place;
  struct node *stmt;
  place = loc_head;

  if (s == NULL) {
  }

  else if (s -> kind == 9) { //param list
    if (strcmp(s->type,  "void") == 0) {
      printf("Typecheck Error: Void param type at line %i\n", s->line);
      exit(1);
    }
    loc_head = make_symbol(s, 'l');
    s = s -> next;
    loc_head -> next = place;
    local = loc_head;
    while (s != NULL) {  //insert declarations at beginning of list
      local -> next = make_symbol(s, 'l');
      local -> next -> next = place;
      local = local -> next;
      s = s -> next;
    }
  }

  else if (s -> kind == 10) { //compound statement
    place = loc_head;
    c = s -> child1; //local decs
    if (c != NULL) {
      loc_head = make_symbol(c, 'l');
      c = c -> next;
      loc_head -> next = place;
      local = loc_head;
      while (c != NULL) {  //insert declarations at beginning of list
	local -> next = make_symbol(c, 'l');
	local -> next -> next = place;
	local = local -> next;
	c = c -> next;
      }
    }
    stmt = s -> child2;
    while (stmt != NULL) { //stmt list
      findRefStmt(stmt);
      stmt = stmt -> next;
    }
    loc_head = place;
  }

  else if (s -> kind == 14) { //exp stmt
    findRefExp(s -> child1);  
  }
  
  else if (s -> kind == 15) { //if stmt
    findRefExp(s -> child1);
    findRefStmt(s -> child2);
    findRefStmt(s -> child3);
  }

  else if (s -> kind == 16) { //while stmt
    findRefExp(s -> child1);
    findRefStmt(s -> child2);
  }
  
  else if (s -> kind == 17) { //return stmt
    findRefExp(s -> child1);
  }

  else if (s -> kind == 18) { //write stmt
    findRefExp(s -> child1);
  }
}

void findRefExp(struct node *e) {
  struct node *arg;
 
  if (e == NULL) {
  }

  else if (e -> kind == 21) { //comp exp
    findRefExp(e -> child1);
    findRefExp(e -> child3);
  }
  
  else if (e -> kind == 38) { //var assign
    findRefExp(e -> child1);
    findRefExp(e -> child2);
  }
  
  else if (e -> kind == 24) { //addop
    findRefExp(e -> child1);
    findRefExp(e -> child2);
  }

  else if (e -> kind == 26) { //mulop
    findRefExp(e -> child1);
    findRefExp(e -> child2);
  }

  else if (e -> kind == 27) { //f node
    if (e -> child1-> kind == 32) {
      find_dec(e->child1);
    }
    else if (e -> child1 -> kind == 35) {
      find_dec(e->child1);
      findRefExp(e->child1->arr_index);
    }
    else if (e -> child1 -> kind == 21) { //(expression)
      findRefExp(e->child1);
    }
    else if (e -> child1 -> kind == 29) { //f'n call
      arg = e -> child1 -> child2;
      find_dec(e->child1);
      if (arg -> kind == 30) { //empty args
      }
      else {
	while (arg != NULL) {
	  findRefExp(arg);
	  arg = arg -> next;
	}
      }
    }
  }

  else if (e -> kind == 36) { //factor pre
    if (strcmp(e->value, "*") == 0) {
      if (e -> child1 -> kind == 32) {
	find_dec(e);
      }
      else {
	printf("Typecheck Error: Cannot have pointer to anything other than <id> (line %i)\n", e->line);
	exit(1);
      }
    }
    else if (strcmp(e->value, "-") == 0) {
      if (e->child1->kind != 33) {
	printf("Typecheck Error: Cannot have a '-' before anything other than a number (line %i)\n", e->line);
	exit(1);
      }
    }
    else { //'&'
      if (e -> child1 -> kind == 32 || e -> child1 -> kind == 35) {
	find_dec(e->child1);
	if (e -> child1 -> kind == 35) {
	  findRefExp(e -> child1 -> arr_index);
	}
      }
      else {
	printf("Typecheck Error: Cannot have anything other than an identifier or an array element after a '&' (line %i)\n", e->line);
	exit(1);
      }
    }
  }

}


void typeCheck(struct node *t) {
  void typeCheckStmt(struct node *s, char *ret_type);
  struct node *dec;
  dec = t -> child1;
  while (dec != NULL) {
    if (dec -> v_or_f == 2) {
      typeCheckStmt(dec -> child2, dec -> type);
    }
    dec = dec -> next;
  }
}


void typeCheckStmt(struct node *s, char *ret_type) {
  void typeCheckExp(struct node *e);
  struct node *stmt;

  if (s == NULL) {
  }

  else if (s -> kind == 10) { //compound statement
    stmt = s -> child2;
    while (stmt != NULL) { //stmt list
      typeCheckStmt(stmt, ret_type);
      stmt = stmt -> next;
    }
  }

  else if (s -> kind == 14) { //exp stmt
    typeCheckExp(s -> child1);  
  }
  
  else if (s -> kind == 15) { //if stmt
    typeCheckExp(s -> child1);
    typeCheckStmt(s -> child2, ret_type);
    typeCheckStmt(s -> child3, ret_type);
  }

  else if (s -> kind == 16) { //while stmt
    typeCheckExp(s -> child1);
    typeCheckStmt(s -> child2, ret_type);
  }
  
  else if (s -> kind == 17) { //return stmt
    typeCheckExp(s -> child1);
    if (s -> child1 != NULL && strcmp(s -> child1 -> type, ret_type) != 0) {
      printf("Typecheck Error: Incorrect return type at line %i. Should be of type %s.\n", s -> line, ret_type);
      exit(1);
    }
  }

  else if (s -> kind == 18) { //write stmt
    typeCheckExp(s -> child1);
  }

}

void typeCheckExp(struct node *e) {
  struct node *p;
  struct node *a;
  char *p_type;
  if (e == NULL) {
  }

  else if (e -> kind == 21) { //comp exp
    typeCheckExp(e -> child1);
    typeCheckExp(e -> child3);
    if (e->child3 != NULL) {
      if (strcmp(e->child1->type, e->child3->type) == 0) {
	e -> type = e -> child1 -> type;
	print_exp(e -> type, e -> line, e -> kind);
      }
      else {
	printf("Typecheck Error: Conflicting types in expression at line %i\n", e->line);
	exit(1);
      }
    }
    else {
      e -> type = e -> child1 -> type;
      print_exp(e -> type, e -> line, e -> kind);
    }
  }
  
  else if (e -> kind == 38) { //var assign
    typeCheckExp(e -> child1);
    typeCheckExp(e -> child2);
    if (strcmp(e->child1->type, e->child2->type) == 0) {
      e -> type = e -> child1 -> type;
      print_exp(e -> type, e -> line, e -> kind);
    }
    else {
      printf("Typecheck Error: Conflicting types in assignment at line %i\n", e->line);
      exit(1);
    }
  }
  
  else if (e -> kind == 24) { //addop
    typeCheckExp(e -> child1);
    typeCheckExp(e -> child2);
    if (strcmp(e->child1->type, "int") == 0 && strcmp(e->child2->type, "int") == 0) {
      e -> type = "int";
      print_exp("int", e -> line, e -> kind);
    }
    else {
      printf("Typecheck Error: Arithmetic expression at line %i contains a non-integer element\n", e -> line);
      exit(1);
    }
  }

  else if (e -> kind == 26) { //mulop
    typeCheckExp(e -> child1);
    typeCheckExp(e -> child2);
    if (strcmp(e->child1->type, "int") == 0 || strcmp(e->child2->type, "int") == 0) {
      e -> type = "int";
      print_exp("int", e -> line, e -> kind);
    }
    else {
      printf("Typecheck Error: Arithmetic expression at line %i contains a non-integer element\n", e -> line);
      exit(1);
    }
  }

  else if (e -> kind == 27) { //f node
    if (e -> child1 -> kind == 33) {
      e -> type = "int";
      print_fact("int", e -> line, e -> child1 -> kind, e -> child1 -> value);
    }
    else if (e -> child1 -> kind == 34) {
      e -> type = "string";
      print_fact("string", e -> line, e -> child1 -> kind, e -> child1 -> value);
    }
    else if (e -> child1-> kind == 32) {
      e -> type = e -> child1 -> dec -> type;
      print_fact(e -> type, e -> line, e -> child1 -> kind, e -> child1 -> value);
    }
    else if (e -> child1 -> kind == 35) {
      if (strcmp(e ->child1->dec->type, "int array") == 0) {
	e -> type = "int";
      }
      else {
	e -> type = "string";
      }
      print_fact(e->type, e -> line, e -> child1 -> kind, e -> child1 -> value);
      typeCheckExp(e -> child1 -> arr_index);
      if (strcmp(e->child1->arr_index->type, "int") != 0) {
	printf("Typecheck Error: Arrays can only be indexed with integer values (line %i)\n", e -> line);
	exit(1);
      }
    }
    else if (e -> child1 -> kind == 21) { //(expression)
      typeCheckExp(e -> child1);
      e -> type = e -> child1 -> type;
    }
    else if (e -> child1 -> kind == 29) { //f'n call
      e -> type = e -> child1 -> dec -> type;
      p = e -> child1 -> dec -> params;
      a = e -> child1 -> child2; //args
      if (a->value != NULL) { //'<empty>'
	if (p->kind == 9) { // not 'void' 
	  printf("Typecheck Error: Missing args in call of %s() at line %i\n", e->child1->child1->value, e->line);
	  exit(1);
	}
      }
      else if (p->value != NULL) {
	printf("Typecheck Error: Unneeded args in call of %s() at line %i\n", e->child1->child1->value, e->line);
	exit(1);
      }
      else {
	while (a != NULL) {
	  if (p == NULL) {
	    printf("Typecheck Error: Too many arguments in call of %s() at line %i\n", e->child1->child1->value, e->line);
	    exit(1);
	  }
	  typeCheckExp(a);
	  p_type = param_type(p);
	  if (strcmp(a->type, p_type) != 0) {
	    printf("Typecheck Error: Incorrect argument in call of %s() type at line %i\n", e->child1->child1->value, e->line);
	    exit(1);
	  }
	  a = a -> next;
	  p = p -> next;
	}
	if (p != NULL) {
	  printf("Typecheck Error: Not enough args in call of %s() at line %i\n", e->child1->child1->value, e->line);
	  exit(1);
	}
      }
      print_fact(e->type, e->line, e->child1->kind, e->child1->child1->value); 
    }
    else { //read call
      e->type = "int";
    }
  }

  else if (e -> kind == 36) { //factor pre
    if (strcmp(e->value, "*") == 0) {
      if (strcmp(e->child1->dec->type, "int pointer") == 0) {
	e -> type = "int";
      }
      else {
	e -> type = "string";
      }
      print_pre(e -> type, e -> line, "*", e -> child1 -> value);
    }
    else if (strcmp(e->value, "-") == 0) {
      e -> type = "int";
      print_pre("int", e -> line, "-", e -> child1 -> value);
    }
    else { // '&'
      if (e -> child1 -> kind == 32) {
	if (strcmp(e->child1->dec->type, "int") == 0) {
	  e -> type = "int pointer";
	}
	else {
	  e -> type = "string pointer";
	}
	print_pre(e->type, e->line, "&",  e -> child1 -> value);
      }
      else {
	if (strcmp(e->child1->dec->type, "int array") == 0) {
	  e -> type = "int pointer";
	}
	else {
	  e -> type = "string pointer";
	}
	print_pre(e->type, e->line, "&", e->child1->value);
	typeCheckExp(e -> child1 -> arr_index);
	if (strcmp(e->child1->arr_index->type, "int") != 0) {
	  printf("Typecheck Error: Arrays can only be indexed with integer values (line %i)\n", e -> line);
	  exit(1);
	}
      }
    }
  }
}

