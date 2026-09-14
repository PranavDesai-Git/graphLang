#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include "TreeNode.h"

struct LocalEnv;

typedef struct {
    void (*registerNative)(char *name, Node *(*func)(Node *args, struct LocalEnv *env));
    Node *(*evaluate)(Node *node, struct LocalEnv *env);
    Node *(*createLiteral)(int value);
    Node *(*createVariable)(char *varName);
    Node *(*createFunction)(Node *funcExpr, Node *args);
    Node *(*createList)(int value, Node *nextNode);
    Node *(*copyTree)(Node *root);
    void (*pushRoot)(Node *node);
    void (*popRoot)(void);

    NodeType (*getNodeType)(Node *n);
    Node *(*getLeft)(Node *n);
    Node *(*getRight)(Node *n);
    int (*getLiteral)(Node *n);
    char *(*getVarName)(Node *n);
    char *(*getFuncName)(Node *n);
} VMAPI;

#endif
