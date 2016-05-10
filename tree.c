/*tree.c
  Michael Clifford
  Define make_node and print_tree*/

#include "tree.h"

struct node *make_node(int lnum, int k, char *nm) {
  struct node *n = malloc(sizeof(struct node));
  if (n == NULL) {
    printf("Malloc Failure\n");
    exit(1);
  }
  n -> kind = k;
  n -> line = lnum;
  n -> name = nm;
  n -> v_or_f = 0;
  n -> is_var = 0;
  n -> arr_len = NULL;
  n -> type = NULL;
  n -> next = NULL;
  n -> value = NULL;
  n -> params = NULL;
  n -> arr_index = NULL;
  n -> child1 = NULL;
  n -> child2 = NULL;
  n -> child3 = NULL;
  n -> dec = NULL;
  return n;
}

struct symbol *make_symbol(struct node *n, char which) {
  struct symbol *s = malloc(sizeof(struct symbol));
  if (s == NULL) { 
    printf("Malloc Failure");
    exit(1);
  }
  s -> next = NULL;
  s -> name = n -> child1 -> value;
  if (strcmp(n->type, "int") == 0) {
    if (n->child1->kind == 32) {
      s -> type = "int";
    }
    else if (n->child1->kind == 35) {
      s -> type = "int array";
    }
    else {
      s -> type = "int pointer";
    }
  }
  else if (strcmp(n->type, "string") == 0) {
    if (n->child1->kind == 32) {
      s -> type = "string";
    }
    else if (n->child1->kind == 35) {
      s -> type = "string array";
    }
    else {
      s -> type = "string pointer";
    }
  }
  else {
    if (n -> v_or_f == 1) {
      printf("Typecheck Error: Variable declared with type void at line %i\n", n->line);
      exit(1);
    }
    else {
      s -> type = "void";
    }
  }
  s -> line = n -> line;
  s -> v_or_f = n -> v_or_f;
  if (n -> v_or_f == 2) {
    s -> params = n -> params;
  }
  s -> to_all = 0;
  s -> depth = -1;
  s -> pos = -1;
  n -> dec = s;
  if (debug == 1) {
    if (which == 'g') {
      printf("Global declaration added: %s, type %s, line %i\n", s->name, s->type, s->line);
    }
    else {
      printf("Local declaration added: %s, type %s, line %i\n", s->name, s->type, s->line);
    }
  }
  
  return s;
}

void print_tree(struct node *n) {
  if ((n->kind >= 1 && n-> kind <= 21) || (n->kind >= 27 && n-> kind < 30) || n ->kind == 38 || n->kind == 37)  {
    if (n -> kind == 7 && n -> value != NULL) { //void param 
      printf("Line %i: %s, %s\n", n->line, n->name, n->value);
    }
    else if (n -> type != NULL) {
      if (n -> v_or_f == 1) { //var dec
	printf("Line %i: VAR_DEC, type: %s\n", n->line, n->type);
      }
      else if (n -> v_or_f == 2) { //fun dec
	printf("Line %i: FUN_DEC, type: %s\n", n->line, n->type);
      }
      else if (n -> kind == 9 || (n -> kind == 7 && n->value == NULL)) {//param
	printf("Line %i: %s, type: %s\n", n->line, n->name, n->type);
      }
      else {
	printf("Line %i: %s, type: %s\n", n->line, n->name, n->type);
      }
    }
    else {
      printf("Line %i: %s\n", n->line, n->name);
    }
  }  
  else if ((n -> kind >= 32 && n -> kind < 37) || (n->kind >= 22 && n->kind <= 26) || n->kind == 30 || n->kind == 39) {
    if (n -> arr_len != NULL) {
      printf("Line %i: %s: %s, length: %s\n", n->line, n->name, n->value, n->arr_len);
    }
    else {
      printf("Line %i: %s: %s\n", n->line, n->name, n->value);
    }
  }
  if (n -> arr_index != NULL) {
    printf("Array Index:\n");
    print_tree(n->arr_index);
    printf("End Array Index\n");
  }
  if (n -> child1 != NULL) {
    print_tree(n->child1);
  }
  if (n -> params != NULL) {
    print_tree(n->params);
  }
  if (n -> child2 != NULL) {
    if (n->kind == 15 || n->kind == 16) {
      printf("Statement Body:\n");
    }
    if (n->kind == 29) {
      printf("Args:\n");
    }
    if (n->v_or_f == 2) {
      printf("Function Body:\n");
    }
    print_tree(n->child2);
    if (n->kind == 15 || n->kind == 16) {
      printf("End Statement Body\n");
    }
    if (n->kind == 29) {
      printf("End Args\n");
    }
    if (n->v_or_f == 2) {
      printf("End Function Body\n");
    }
    if (n->kind == 38) {
      printf("End Var Assign\n");
    }
  }
  if (n -> child3 != NULL) {
    if (n->kind == 15) {
      printf("Else Statement\n");
    }
    print_tree(n->child3);
    if (n->kind == 15) {
      printf("End Else Statement\n");
    }
  }
  if (n -> next != NULL) {
    print_tree(n->next);
  }
}

void print_glob() {
  struct symbol *place;
  place = glob_head;
  while (place != NULL) {
    printf("{%s, %s, %i}, ", place->name, place->type, place->line);
    place = place -> next;
  }
  printf("\n");
}

void print_loc() {
  struct symbol *place;
  place = loc_head;
  while (place != NULL) {
    printf("{%s, %s, %i}, ", place->name, place->type, place->line);
    place = place -> next;
  }
  printf("\n");
}

void find_dec(struct node *n) {
  struct symbol *l;
  struct symbol *g;
  int i = 0;
  l = loc_head;
  g = glob_head;

  if (n -> kind == 32) {
    while (l != NULL) {
      if (l->v_or_f != 2 && strcmp(l->name, n->value) == 0 && strcmp(l->type, "void") != 0) {
	n -> dec = l;
	if (debug == 1) {
	  printf("Identifier %s at line %i linked to local declaration of type %s at line %i\n", n->value, n->line, l->type, l->line);
	}
	i = 1;
	break;
      }
      l = l -> next;
    }
    if (i == 0) {
      while (g != NULL) {
	if (g->v_or_f != 2 && strcmp(g->name, n->value) == 0 && strcmp(g->type, "void") != 0) {
	  n -> dec = g;
	  i = 1;
	  if (debug == 1) {
	    printf("Identifier %s at line %i linked to global declaration of type %s at line %i\n", n->value, n->line, g->type, g->line);
	  }
	  break;
	}
	g = g -> next;
      }
    }
    if (i == 0) {
      printf("Typecheck Error: Failed to link identifier %s at line %i to a declaration\n", n->value, n->line);
      exit(1);
    }
  }

  else if (n -> kind == 35) {
    while (l != NULL) {
      if (strcmp(l->name, n->value) == 0 && (strcmp(l->type, "int array") == 0 || strcmp(l->type, "string array") == 0)) {
	n -> dec = l;
	if (debug == 1) {
	  printf("Array Identifier %s at line %i linked to local declaration of type %s at line %i\n", n->value, n->line, l->type, l->line);
	}
	i = 1;
	break;
      }
      l = l -> next;
    }
    if (i == 0) {
      while (g != NULL) {
	if (strcmp(g->name, n->value) == 0 && (strcmp(g->type, "int array") == 0 || strcmp(g->type, "string array") == 0)) {
	  n -> dec = g;
	  i = 1;
	  if (debug == 1) {
	    printf("Array Identifier %s at line %i linked to global declaration of type %s at line %i\n", n->value, n->line, g->type, g->line);
	  }
	  break;
	}
	g = g -> next;
      }
    }
    if (i == 0) {
      printf("Typecheck Error: Failed to link identifier %s at line %i to a declaration\n", n->value, n->line);
      exit(1);
    }
  }

  else if (n -> kind == 29) {
    while (l != NULL) {
      if (l->v_or_f == 2 && strcmp(l->name, n->child1->value) == 0) {
	n -> dec = l;
	if (debug == 1) {
	  printf("Function %s at line %i linked to local declaration of return type %s at line %i\n", n->child1->value, n->line, l->type, l->line);
	}
	i = 1;
	break;
      }
      l = l -> next;
    }
    if (i == 0) {
      while (g != NULL) {
	if (g->v_or_f == 2 && strcmp(g->name, n->child1->value) == 0) {
	  n -> dec = g;
	  i = 1;
	  if (debug == 1) {
	    printf("Function %s at line %i linked to global declaration of return type %s at line %i\n", n->child1->value, n->line, g->type, g->line);
	  }
	  break;
	}
	g = g -> next;
      }
    }
    if (i == 0) {
      printf("Typecheck Error: Failed to link funcion %s at line %i to a declaration\n", n->child1->value, n->line);
      exit(1);
    }
  }

  else if (n -> kind == 36) { //'*'
    while (l != NULL) {
      if (strcmp(l->name, n->child1->value) == 0 && (strcmp(l->type, "int pointer") == 0 || strcmp(l->type, "string pointer") == 0)) {
	n-> child1 -> dec = l;
	if (debug == 1) {
	  printf("Pointer %s at line %i linked to local declaration of type %s at line %i\n", n->child1->value, n->line, l->type, l->line);
	}
	i = 1;
	break;
      }
      l = l -> next;
    }
    if (i == 0) {
      while (g != NULL) {
	if (g->v_or_f == 1 && strcmp(g->name, n->child1->value) == 0 && (strcmp(g->type, "int pointer") == 0 || strcmp(g->type, "string pointer") == 0)) {
	  n -> child1 -> dec = g;
	  i = 1;
	  if (debug == 1) {
	    printf("Pointer %s at line %i linked to global declaration of type %s at line %i\n", n->child1->value, n->line, g->type, g->line);
	  }
	  break;
	}
	g = g -> next;
      }
    }
    if (i == 0) {
      printf("Typecheck Error: Failed to link pointer %s at line %i to a declaration\n", n->child1->value, n->line);
      exit(1);
    }
  }
}

void print_exp(char *type, int line, int kind) {
  if (debug == 1) {
    if (kind == 21) {
      printf("Expression at line %i assigned type %s\n", line, type);
    }
    else if (kind == 38) {
      printf("Assignment at line %i assigned type %s\n", line, type);
    }
    else {
      printf("Arithmetic Expression at line %i assigned type %s\n", line, type);
    }
  }
}

void print_fact(char *type, int line, int kind, char *value) {
  if (debug == 1) {
    if (kind == 33 || kind == 34) {
      printf("Constant %s at line %i assigned type %s\n", value, line, type);
    }
    else if (kind == 32) {
      printf("Variable %s at line %i assigned type %s\n", value, line, type);
    }
    else if (kind == 35) {
      printf("Array Element %s at line %i assigned type %s\n", value, line, type);
    }
    else {
      printf("Function Call %s() at line %i assigned type %s\n", value, line, type);
    }
  }
}

void print_pre(char *type, int line, char *val, char *value) {
  if (debug == 1) {
    if (strcmp(val, "*") == 0) {
      printf("Pointer %s at line %i assigned type %s\n", value, line, type);
    }
    else if (strcmp(val, "-") == 0) {
      printf("Constant %s at line %i assigned type %s\n", value, line, type);
    }
    else {
      printf("Dereference of %s at line %i assigned type %s\n", value, line, type);
    }
  }
}

char *param_type(struct node *p) {
  if (strcmp(p->type, "int") == 0) {
    if (p->child1->kind == 32) {
      return "int";
    }
    else if (p->child1->kind == 35) {
      return "int array";
    }
    else {
      return "int pointer";
    }
  }
  else {
    if (p->child1->kind == 32) {
      return "string";
    }
    else if (p->child1->kind == 35) {
      return "string array";
    }
    else {
      return "string pointer";
    }
  }
}

void free_tree(struct node *t) {
  if (t == NULL) {
  }
  else {
    free_tree(t -> next);
    free_tree(t -> params);
    free_tree(t -> arr_index);
    free_tree(t -> child1);
    free_tree(t -> child2);
    free_tree(t -> child3);
    free(t);
  }
}

void free_symbol(struct symbol *s) {
  if (s == NULL) {
  }
  else {
    free_symbol(s -> next);
    free_symbol(s);
  }
}

void free_glob() {
  struct symbol *g;
  struct symbol *h;
  g = glob_head;
  if (g != NULL) {
    h = g -> next;
    free(g);
    while (h != NULL) {
      g = h;
      h = g -> next;
      free(g);
    }
  }
}
