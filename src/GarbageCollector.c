#include "GarbageCollector.h"
#include "Environment.h"
#include "TreeNodePrivate.h"
#include <stdio.h>
#include <stdlib.h>

int gcEnabled = 0;
void enableGC(void) { gcEnabled = 1; }

#define MAX_ROOTS 65536
Node *gcRoots[MAX_ROOTS];
int rootCount = 0;

void pushRoot(Node *node) {
    if (rootCount < MAX_ROOTS) {
        gcRoots[rootCount++] = node;
    } else {
        printf("Fatal Error: Shadow Stack Overflow!\n");
        exit(1);
    }
}

void popRoot(void) {
    if (rootCount > 0)
        rootCount--;
}

void markNode(Node *n) {
    if (n == NULL)
        return;
    if (n->statusFlags & FLAG_GC_MARKD)
        return;

    n->statusFlags |= FLAG_GC_MARKD;
    if (n->type == LITERAL || n->type == GLOBAL_VAR || n->type == LOCAL_VAR)
        return;

    if (n->type == ENV_FRAME) {
        int count = n->errorFlags;
        for (int i = 0; i < count; i++) {
            markNode(n->data.locals[i]);
        }
    } else if (n->type == CLOSURE) {
        markNode(n->data.closure);
    }

    markNode(n->left);
    markNode(n->right);
}

void markAll(void) {
    for (int i = 0; i < rootCount; i++) {
        markNode(gcRoots[i]);
    }

    for (int i = 0; i < ENV_SIZE; i++) {
        EnvEntry *temp = envTable[i];

        while (temp != NULL) {
            if (temp->val != NULL) {
                markNode(temp->val);
            }
            temp = temp->next;
        }
    }
}
