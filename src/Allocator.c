#include "GarbageCollector.h"
#include "TreeNodePrivate.h"
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 1024

void sweep(void);

typedef struct Chunk {
    Node nodes[CHUNK_SIZE];
    struct Chunk *next;
} Chunk;

Chunk *first = NULL;
Chunk *current = NULL;
int top = 0;
Node *freeListHead = NULL;

Chunk *createChunk(void) {
    Chunk *newChunk = malloc(sizeof(Chunk));
    if (newChunk == NULL) {
        printf("CRITICAL ERROR: OS out of memory!\n");
        exit(1);
    }
    newChunk->next = NULL;
    return newChunk;
}

void initAllocator(void) {
    first = createChunk();
    current = first;
    top = 0;
    freeListHead = NULL;
}

void pushFreeList(Node *deadNode) {
    deadNode->left = freeListHead;
    freeListHead = deadNode;
}

Node *popFreeList(void) {
    if (freeListHead == NULL)
        return NULL;
    Node *temp = freeListHead;
    freeListHead = freeListHead->left;
    return temp;
}

Node *allocNode(void) {
    Node *newNode;
    Node *recycled = popFreeList();

    if (recycled == NULL && gcEnabled) {
        markAll();
        sweep();
        recycled = popFreeList();
    }

    if (recycled != NULL) {
        newNode = recycled;
    } else {
        if (current == NULL) {
            initAllocator();
        }
        if (top >= CHUNK_SIZE) {
            Chunk *newChunk = createChunk();
            current->next = newChunk;
            current = newChunk;
            top = 0;
        }
        newNode = &current->nodes[top++];
    }

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->infoFlags = 0;
    newNode->errorFlags = 0;
    newNode->statusFlags = FLAG_ALLOCATED;

    return newNode;
}

void freeAllChunks(void) {
    Chunk *temp = first;
    Chunk *freeChunk;
    while (temp != NULL) {
        freeChunk = temp;
        temp = temp->next;
        free(freeChunk);
    }
}

void sweep(void) {
    freeListHead = NULL;
    Chunk *temp = first;
    while (temp != NULL) {
        int limit = (temp == current) ? top : CHUNK_SIZE;
        for (int i = 0; i < limit; ++i) {
            Node *tempNode = &temp->nodes[i];
            if (tempNode->statusFlags & FLAG_ALLOCATED) {
                if (tempNode->statusFlags & FLAG_GC_MARKD) {
                    tempNode->statusFlags &= ~FLAG_GC_MARKD;
                } else {
                    tempNode->statusFlags &= ~FLAG_ALLOCATED;
                    pushFreeList(tempNode);
                }
            }
        }
        temp = temp->next;
    }
}
