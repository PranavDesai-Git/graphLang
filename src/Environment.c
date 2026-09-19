#define _POSIX_C_SOURCE 200809L
#include "Environment.h"
#include "TreeNode.h"
#include <stdlib.h>
#include <string.h>

int envSize = 1024;
int envCount = 0;
EnvEntry **envTable = NULL;

void initEnvironment(void) {
    envTable = calloc(envSize, sizeof(EnvEntry*));
}

unsigned long hashString(char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % envSize;
}

void envResize(void) {
    int oldSize = envSize;
    EnvEntry **oldTable = envTable;
    
    envSize *= 2;
    envTable = calloc(envSize, sizeof(EnvEntry*));
    
    for (int i = 0; i < oldSize; i++) {
        EnvEntry *temp = oldTable[i];
        while (temp != NULL) {
            EnvEntry *next = temp->next;
            int pos = hashString(temp->key);
            temp->next = envTable[pos];
            envTable[pos] = temp;
            temp = next;
        }
    }
    free(oldTable);
}

void envInsert(EnvEntry *newEntry) {
    if (envCount >= envSize * 0.75) {
        envResize();
    }
    int pos = hashString(newEntry->key);

    if (envTable[pos] == NULL) {
        envTable[pos] = newEntry;
        envCount++;
        return;
    }

    EnvEntry *temp = envTable[pos];
    while (temp != NULL) {
        if (strcmp(temp->key, newEntry->key) == 0) {
            temp->val = newEntry->val;
            free(newEntry->key);
            free(newEntry);
            return;
        }

        if (temp->next == NULL) {
            break;
        }
        temp = temp->next;
    }

    temp->next = newEntry;
    envCount++;
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
    entry->key = strdup(name);
    entry->val = value;
    entry->next = NULL;
    envInsert(entry);
}

void registerNative(char *name, void *cFunc) {
    defineVariable(name, createNativeFunc(cFunc));
}
