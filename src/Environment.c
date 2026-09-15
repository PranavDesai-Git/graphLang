#include "Environment.h"
#include "TreeNode.h"
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
void defineVariable(char *name, Node *value) {
    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->key = name;
    entry->val = value;
    entry->next = NULL;
    envInsert(entry);
}

void registerNative(char *name, void *cFunc) {
    defineVariable(name, createNativeFunc(cFunc));
}
