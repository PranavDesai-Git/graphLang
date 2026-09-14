#ifndef TREENODE_H
#define TREENODE_H

typedef enum { LITERAL, VARIABLE, FUNCTION, LIST } NodeType;

typedef struct Node Node;

/*
struct Node *left;
struct Node *right;

union {
    int literal;
    char *var;
    char *func;
    int listLiteral;
} data;

NodeType type;

unsigned int infoFlags;
unsigned int errorFlags;
unsigned int statusFlags;
*/

NodeType getNodeType(Node *n);
Node *getLeft(Node *n);
Node *getRight(Node *n);
int getLiteral(Node *n);
char *getVarName(Node *n);
char *getFuncName(Node *n);

void setLeft(Node *n, Node *left);
void setRight(Node *n, Node *right);

Node *createLiteral(int value);
Node *createVariable(char *varName);
Node *createFunction(Node *funcExpr, Node *args);
Node *createList(int value, Node *nextNode);
Node *copyTree(Node *root);
Node *substitute(Node *root, char *paramName, Node *argValue);
Node *createArgs1(Node *arg1);
Node *createArgs2(Node *arg1, Node *arg2);
Node *createArgs3(Node *arg1, Node *arg2, Node *arg3);
#endif // TREENODE_H
