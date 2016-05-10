/*type_checker.h
Michael Clifford
*/
#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <stdio.h>
#include <stdlib.h>

void findRef(struct node *t);
void findRefStmt(struct node *s);
void findRefExp(struct node *e);

void typeCheck(struct node *t);
void typeCheckStmt(struct node *s, char *ret_type);
void typeCheckExp(struct node *e);

#endif
