/*compiler.c
  Michael Clifford
  runs the whole compiler
*/

#include "token.h"
#include "tree.h"
#include "parser.h"
#include "type_checker.h"
#include "code_help.h"
#include "code_gen.h"

int main(int argc, char *argv[]) {
  int parse = 0;
  if (argc == 1) {
    printf("Error: No file given\n");
    exit(1);
  }
  f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("Error: File not found\n");
    exit(1);
  }
  if (argc == 2) {
    debug = 0;
  }
  else if (strcmp(argv[2], "p") == 0) {
    parse = 1;
  }
  else if (strcmp(argv[2], "db") == 0) {
    debug = 1;
  }
  else if (strcmp(argv[2], "dbp") == 0) {
    debug = 1;
    parse = 1;
  }
  ch = fgetc(f);
  l = 1;
  currtok = getNextToken();
  tree = program();
  if (parse == 1) {
    print_tree(tree);
  }
  loc_head = NULL;
  glob_head = NULL;
  findRef(tree);
  typeCheck(tree);
  findDepthDec(tree->child1, 0, 0);
  genCode(tree->child1);
  free_glob();
  free_symbol(loc_head);
  free_tree(tree);
}
