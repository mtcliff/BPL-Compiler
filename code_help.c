/*code_help.c
  Michael Clifford
  Implement heloer functions for code generation*/

#include "code_help.h"

void glob_var(FILE *file, char *name) {
  char s[50];
  sprintf(s, ".comm %s, 8, 32     #allocate 8-byte variable\n", name);
  fputs(s, file);
}

void glob_arr(FILE *file, char *name, char *size) {
  char s[50];
  long len;
  char *ptr;
  len = strtol(size, &ptr, 10);
  len = len * 8;
  sprintf(s, ".comm %s, %li, 32     #allocate %li byte array \n", name, len, len);
  fputs(s, file);
} 

void header1(FILE *file) {
  fputs(".section .rodata\n", file);
  fputs(".WriteIntStr: .string \"%d \"\n", file);
  fputs(".WritelnStr: .string \"\\n\"\n", file);
  fputs(".WriteStrStr: .string \"%s \"\n", file);
  fputs(".ReadIntStr: .string \"%d\"\n", file);
}

void header2(FILE *file) {
  fputs("\n", file);
  fputs(".text\n", file);
  fputs(".globl main\n", file);
  fputs("\n", file);
}

char *label() {
  char *label = malloc(15);
  sprintf(label, ".L%i", label_count);
  label_count++;
  return label;
}

void print_label(FILE *file, char *l) {
  char *lab = malloc(15);
  sprintf(lab, "%s:\n", l);
  fputs(lab, file);
  free(lab);
}

char *string_lab(int n) {
  char *label = malloc(15);
  sprintf(label, ".S%i", n);
  return label;
}

void std_line(FILE *file, char *op, char *src, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s %s, %s     #%s\n", op, src, dst, cmt);
  fputs(s, file);
}

void std_dollar(FILE *file, char *op, char *src, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s $%s, %s     #%s\n", op, src, dst, cmt);
  fputs(s, file);
}

void std_num_line(FILE *file, char *op, int src, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s $%i, %s     #%s\n", op, src, dst, cmt);
  fputs(s, file);
}

void one_line(FILE *file, char *op, char *x, char *cmt) {
  char s[100];
  sprintf(s, "\t%s %s     #%s\n", op, x, cmt);
  fputs(s, file);
}

void no_line(FILE *file, char *op, char *cmt) {
  char s[100];
  sprintf(s, "\t%s     #%s\n", op, cmt);
  fputs(s, file);
}

void off_src(FILE *file, char *op, int off, char *src, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s %i(%s), %s     #%s\n", op, off, src, dst, cmt);
  fputs(s, file);
}

void off_dst(FILE *file, char *op, char *src, int off, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s %s, %i(%s)     #%s\n", op, src, off, dst, cmt);
  fputs(s, file);
}

void off_dollar(FILE *file, char *op, char *src, int off, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s $%s, %i(%s)     #%s\n", op, src, off, dst, cmt);
  fputs(s, file);
}

void off_both(FILE *file, char *op, int off1, char *src, int off2, char *dst, char *cmt) {
  char s[100];
  sprintf(s, "\t%s %i(%s), %i(%s)     #%s\n", op, off1, src, off2, dst, cmt);
  fputs(s, file);
}
