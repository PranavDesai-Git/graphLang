#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "TreeNode.h"
#include <stdint.h>

#define ENV_SIZE 1024

typedef Node *(*Func)(Node *args, Node *env);

typedef struct EnvEntry {
    char *key;
    Node *params;        // A LIST node of VARIABLE nodes
    uint_fast8_t isFunc; // 1 if is func 0 if var

    union {
        Node *node;
        Func func;
    } val;

    struct EnvEntry *next; // in case of collisions
} EnvEntry;

extern EnvEntry *envTable[ENV_SIZE];

void envInsert(EnvEntry *entry);
EnvEntry *getEnvEntry(char *key);
void defineVariable(char *name, Node *val);
void defineFunction(char *name, Node *params, Node *body);
void registerNative(char *name, Func cFunc);

#endif
