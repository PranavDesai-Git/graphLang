#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include "TreeNode.h"

struct LocalEnv;
typedef int Handle;

typedef struct {
    void (*registerNative)(char *name, Handle (*func)(Handle args, Handle env));
    Handle (*evaluate)(Handle node, Handle env);
    Handle (*createLiteral)(int value);
    Handle (*createGlobalVar)(char *varName);
    Handle (*createLocalVar)(int depth, int index);
    Handle (*createFunction)(Handle funcExpr, Handle args);
    Handle (*createList)(int value, Handle nextNode);
    Handle (*copyTree)(Handle root);
    void (*pushRoot)(Handle node);
    void (*popRoot)(void);

    NodeType (*getNodeType)(Handle n);
    Handle (*getLeft)(Handle n);
    Handle (*getRight)(Handle n);
    int (*getLiteral)(Handle n);
    char *(*getVarName)(Handle n);
    char *(*getFuncName)(Handle n);
} VMAPI;

#endif
