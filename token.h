/*token.h
  Michael Clifford
  Define token struct and declare getNextToken() alond with its helper functions*/

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct token {
  int kind;
  char *value;
  int line;
};

int ch;
int l;
FILE *f;

struct token *getNextToken();
int iskey(char *x);
int key_kind(char *x);
int spec_kind(char *x);

struct token *currtok;

#endif
