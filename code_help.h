/*code_help.h
  Michael Clifford
  declare helper functions for code generation*/

#ifndef CODE_HELP_H
#define CODE_HELP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void glob_var(FILE *file, char *name);
void glob_arr(FILE *file, char *name, char *size);

void header1(FILE *file);
void header2(FILE *file);
char *label();
int label_count;   //current label number
void print_label(FILE *file, char *l);

char *string_lab(int n);

void std_line(FILE *file, char *op, char *src, char *dst, char *cmt);
void std_dollar(FILE *file, char *op, char *src, char *dst, char *cmt);
void std_num_line(FILE *file, char *op, int src, char *dst, char *cmt); 
void one_line(FILE *file, char *op, char *x, char *cmt);
void no_line(FILE *file, char *op, char *cmt);
void off_src(FILE *file, char *op, int off, char *src, char *dst, char *cmt); 
void off_dst(FILE *file, char *op, char *src, int off, char *dst, char *cmt);
void off_dollar(FILE *file, char *op, char *src, int off, char *dst, char *cmt);
void off_both(FILE *file, char *op, int off1, char *src, int off2, char *dst, char *cmt);

#endif
