/* scanner.c
   Michael Clifford
   implement scanner portion of BPL compiler */

#include "token.h"

int main(int argc, char *argv[]) {
  void print_token(struct token *t);
  if (argc == 1) {
    printf("Error: No file given\n");
    exit(1);
  }
  f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("Error: File not found\n");
    exit(1);
  }
  ch = fgetc(f);
  l = 1;
  struct token *curr = NULL;
  while (ch != EOF) {
    curr = getNextToken();
    print_token(curr);
  }
}

void print_token(struct token *t) {
  printf("Token %i, string %s, line number %i\n", t -> kind, t -> value, t -> line);
}
