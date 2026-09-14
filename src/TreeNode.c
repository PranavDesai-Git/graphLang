#include "Allocator.h"
#include "GarbageCollector.h"
#include "TreeNodePrivate.h"
#include <string.h>

Node *createLiteral(int value) {
    Node *n = allocNode();
    n->type = LITERAL;
    n->data.literal = value;
    return n;
}

Node *createVariable(char *varName) {
    Node *n = allocNode();
    n->type = VARIABLE;
    n->data.var = varName;
    return n;
}

Node *createFunction(Node *funcExpr, Node *args) {
    Node *n = allocNode();
    n->type = FUNCTION;
    n->left = funcExpr;
    n->right = args;
    return n;
}

Node *createList(int value, Node *nextNode) {
    Node *n = allocNode();
    n->type = LIST;
    n->data.listLiteral = value;
    n->right = nextNode;
    return n;
}

Node *copyTree(Node *root) {
    if (root == NULL)
        return NULL;
    Node *clone = allocNode();
    pushRoot(clone);
    clone->type = root->type;
    clone->data = root->data;

    clone->left = copyTree(root->left);
    clone->right = copyTree(root->right);
    popRoot();

    return clone;
}

Node *substitute(Node *root, char *paramName, Node *argValue) {
    if (root == NULL)
        return NULL;

    if (root->type == VARIABLE && strcmp(root->data.var, paramName) == 0) {
        return argValue;
    }

    root->left = substitute(root->left, paramName, argValue);
    root->right = substitute(root->right, paramName, argValue);

    return root;
}

Node *createArgs1(Node *arg1) {
    Node *l1 = createList(0, NULL);
    l1->left = arg1;
    return l1;
}

Node *createArgs2(Node *arg1, Node *arg2) {
    Node *l2 = createList(0, NULL);
    l2->left = arg2;
    Node *l1 = createList(0, l2);
    l1->left = arg1;
    return l1;
}

Node *createArgs3(Node *arg1, Node *arg2, Node *arg3) {
    Node *l3 = createList(0, NULL);
    l3->left = arg3;
    Node *l2 = createList(0, l3);
    l2->left = arg2;
    Node *l1 = createList(0, l2);
    l1->left = arg1;
    return l1;
}

NodeType getNodeType(Node *n) { return n->type; }
Node *getLeft(Node *n) { return n->left; }
Node *getRight(Node *n) { return n->right; }
int getLiteral(Node *n) { return n->data.literal; }
char *getVarName(Node *n) { return n->data.var; }
char *getFuncName(Node *n) { return n->data.func; }

void setLeft(Node *n, Node *left) { n->left = left; }
void setRight(Node *n, Node *right) { n->right = right; }
