/*token.c
  Michael Clifford
  Implement getNextToken() along with its helper functions*/

#include "token.h"

struct token *getNextToken() {
  int iskey(char *x);
  int key_kind(char *x);
  int spec_kind(char *x);
  char *str = malloc(100);
  int i = 0;
  int d = 0;
  while (isspace(ch) && ch != '\n') {
    ch = fgetc(f);
  }
  if (ch == '\n' || ch ==EOF) {
    if (ch == '\n') {                //if end of line, gp to the next one
      l = l + 1;
      ch = fgetc(f);
      return getNextToken(/*ch, l, f*/);
    }
    else {
      struct token *eof = malloc(100);             //if EOF, make EOF token
      eof -> kind = 0;
      eof -> value = "";
      eof -> line = l-1;
      return eof;
    }
  }
  
  else {
    if (isdigit(ch)) {           //get number token
      while (ch != '\n' && ch != EOF && isdigit(ch)) {
	*(str + i) = ch;
	i++;
	ch = fgetc(f);
      }
      *(str + i) = '\0';
      struct token *d = malloc(100);
      d -> kind = 33;
      d -> value = str;
      d -> line = l;
      return d;
    }

    else if (isalpha(ch)) {       //get identifier or keyword token
      while (ch != '\n' && ch != EOF && (isalpha(ch) || isdigit(ch) || ch == '_')) {
	*(str + i) = ch;
	i++;
	ch = fgetc(f);
      }
      *(str + i) = '\0';
      struct token *id = malloc(100);
      id -> value = str;
      id -> line = l;
      if (iskey(str)) {
	id -> kind = key_kind(str);
      }
      else {
	id -> kind = 32;
      }
      return id;
    }
    
    else if (ch == '"') {         //get string token
      *str = '"';
      i++;
      ch = fgetc(f);
      while (ch != '\n' && ch != EOF && ch != '"') {
	*(str + i) = ch;
	i++;
	ch = fgetc(f);
      }
      if (ch != '"') {
	printf("Error: Missing closing quotation\n");
	exit(1);
      }
      else {
	*(str + i) = ch;
	i++;
	*(str + i) = '\0';
	struct token *s = malloc(100);
	s -> kind = 34;
	s -> value = str;
	s -> line = l;
	ch = fgetc(f);
	return s;
      }
    }
    
    else {              //get special symbol token
      d = fgetc(f);
      if (ch == '/' && d == '*') {  //skip over comments
	while (2 > 1) {
	  if (ch == '*' && d == '/') {
	    break;
	  }
	  else if (ch == EOF) {
	    printf("Error: Missing closing comment marker\n");
	    exit(1);
	  }
	  else {
	    if (d == '\n') {
	      l = l + 1;
	    }
	    ch = d;
	    d = fgetc(f);
	  }
	}
	ch = fgetc(f);
	return getNextToken(/*ch, l, f*/);
      }
      else {
	ungetc(d, f);
	*str = ch;
	i++;
	ch = fgetc(f);
	if (*str == '<' || *str == '=' || *str == '!' || *str == '>') {
	  if (ch == '=') {
	    *(str + i) = ch;
	    i++;
	    ch = fgetc(f);
	  }
	}
	*(str + i) = '\0';
	struct token *spec = malloc(100);
	spec -> value = str;
	spec -> line = l;
	spec -> kind = spec_kind(str);
	return spec;
      }
    }
  }
  free(str);
}

int iskey(char *x) {     //determine if string is a keyword
  if (strcmp(x, "int") == 0 || strcmp(x, "void") == 0 || strcmp(x, "string") == 0 || strcmp(x, "if") == 0 || strcmp(x, "else") == 0 || strcmp(x, "while") == 0 || strcmp(x, "return") == 0 || strcmp(x, "write") == 0 || strcmp(x, "writeln") == 0 || strcmp(x, "read") == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

int key_kind(char *x) {           //return kind number for keyword
  if (strcmp(x, "int") == 0) {
    return 1;
  }
  else if (strcmp(x, "void") == 0) {
    return 2; 
  }
  else if (strcmp(x, "string") == 0) {
    return 3; 
  }
  else if (strcmp(x, "if") == 0) {
    return 4; 
  }
  else if (strcmp(x, "else") == 0) {
    return 5; 
  }
  else if (strcmp(x, "while") == 0) {
    return 6; 
  }
  else if (strcmp(x, "return") == 0) {
    return 7; 
  }
  else if (strcmp(x, "write") == 0) {
    return 8; 
  }
  else if (strcmp(x, "writeln") == 0) {
    return 9; 
  }
  else {    //=="read"
    return 10; 
  }
}
  
int spec_kind(char *x) {      //return kind number for special symbol
  if (strcmp(x, ";") == 0) {
    return 11;
  }
  else if (strcmp(x, ",") == 0) {
    return 12;
  }
  else if (strcmp(x, "[") == 0) {
    return 13;
  }
  else if (strcmp(x, "]") == 0) {
    return 14;
  }
  else if (strcmp(x, "{") == 0) {
    return 15;
  }
  else if (strcmp(x, "}") == 0) {
    return 16;
  }
  else if (strcmp(x, "(") == 0) {
    return 17;
  }
  else if (strcmp(x, ")") == 0) {
    return 18;
  }
  else if (strcmp(x, "<") == 0) {
    return 19;
  }
  else if (strcmp(x, "<=") == 0) {
    return 20;
  }
  else if (strcmp(x, "==") == 0) {
    return 21;
  }
  else if (strcmp(x, "!=") == 0) {
    return 22;
  }
  else if (strcmp(x, ">=") == 0) {
    return 23;
  }
  else if (strcmp(x, ">") == 0) {
    return 24;
  }
  else if (strcmp(x, "+") == 0) {
    return 25;
  }
  else if (strcmp(x, "-") == 0) {
    return 26;
  }
  else if (strcmp(x, "*") == 0) {
    return 27;
  }
  else if (strcmp(x, "/") == 0) {
    return 28;
  }
  else if (strcmp(x, "%") == 0) {
    return 29;
  }
  else if (strcmp(x, "=") == 0) {
    return 30;
  }
  else if (strcmp(x,  "&") == 0) {
    return 31;
  }
  else {
    printf("Error: Symbol not recognized\n");
    exit(1);
  }
}
