#ifndef TREENODEPRIVATE_H
#define TREENODEPRIVATE_H
#include "TreeNode.h"
struct Node {
    struct Node *left;
    struct Node *right;

    union {
        int literal;
        char *var;
        int index;
        char *call;
        int consLiteral;
        struct Node **locals;
        void *userdata;
        struct Node *closure;
        struct Node *nativeFunc;
    } data;

    NodeType type;

    unsigned int infoFlags;
    unsigned int errorFlags;
    unsigned int statusFlags;
};
#endif
