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
        char *func;
        int listLiteral;
        struct Node **locals;
        void *userdata;
    } data;

    NodeType type;

    unsigned int infoFlags;
    unsigned int errorFlags;
    unsigned int statusFlags;
};
#endif
