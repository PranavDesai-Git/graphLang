#include "Environment.h"
#include <stdlib.h>
#include <string.h>

EnvEntry *envTable[ENV_SIZE];

unsigned long hashString(char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % ENV_SIZE;
}

void envInsert(EnvEntry *newEntry) {
    int pos = hashString(newEntry->key);

    if (envTable[pos] == NULL) {
        envTable[pos] = newEntry;
        return;
    }

    EnvEntry *temp = envTable[pos];
    while (temp != NULL) {
        if (strcmp(temp->key, newEntry->key) == 0) {
            temp->val = newEntry->val;
            temp->isFunc = newEntry->isFunc;
            temp->params = newEntry->params;
            free(newEntry);
            return;
        }

        if (temp->next == NULL) {
            break;
        }
        temp = temp->next;
    }

    temp->next = newEntry;
}

EnvEntry *getEnvEntry(char *key) {
    int pos = hashString(key);
    EnvEntry *temp = envTable[pos];

    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void registerNative(char *name, Func cFunc) {
    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->key = name;
    entry->isFunc = 1;
    entry->val.func = cFunc;
    envInsert(entry);
}

void defineVariable(char *name, Node *value) {
    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->key = name;
    entry->params = NULL;
    entry->isFunc = 0;
    entry->val.node = value;
    entry->next = NULL;
    envInsert(entry);
}

void defineFunction(char *name, Node *params, Node *body) {
    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->key = name;
    entry->params = params;
    entry->isFunc = 2; // User-defined!
    entry->val.node = body;
    entry->next = NULL;
    envInsert(entry);
}

