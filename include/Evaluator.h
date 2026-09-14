#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "TreeNode.h"
#include "Environment.h"

Node *evaluate(Node *node, Node *env);
Node *executeIO(Node *action);
Node *copyTree(Node *root);
Node *substitute(Node *root, char *paramName, Node *argValue);

#endif
