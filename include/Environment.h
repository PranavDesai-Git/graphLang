#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "TreeNode.h"
#include <stdint.h>

#define ENV_SIZE 1024

typedef struct EnvEntry {
    char *key;
    Node *val;
    struct EnvEntry *next;
} EnvEntry;

extern EnvEntry *envTable[ENV_SIZE];

void envInsert(EnvEntry *entry);
EnvEntry *getEnvEntry(char *key);
void defineVariable(char *name, Node *val);
void registerNative(char *name, void *cFunc);

#endif
