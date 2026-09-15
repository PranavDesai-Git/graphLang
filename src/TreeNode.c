#include "TreeNode.h"
#include "Allocator.h"
#include "GarbageCollector.h"
#include "TreeNodePrivate.h"
#include <stdlib.h>
#include <string.h>

Node *createLiteral(int value) {
    Node *n = allocNode();
    n->type = LITERAL;
    n->data.literal = value;
    return n;
}

Node *createGlobalVar(char *varName) {
    Node *n = allocNode();
    n->type = GLOBAL_VAR;
    n->data.var = varName;
    return n;
}

Node *createLocalVar(int depth, int index) {
    Node *n = allocNode();
    n->type = LOCAL_VAR;
    n->infoFlags = depth;
    n->data.index = index;
    return n;
}

Node *createCall(Node *funcExpr, Node *args) {
    Node *n = allocNode();
    n->type = CALL;
    n->left = funcExpr;
    n->right = args;
    return n;
}

Node *createClosure(Node *params, Node *body, Node *env) {
    Node *n = allocNode();
    n->type = CLOSURE;
    n->left = params;
    n->right = body;
    n->data.closure = env;
    return n;
}

Node *createNativeFunc(void *cFunc) {
    Node *n = allocNode();
    n->type = NATIVE_FUNC;
    n->data.userdata = cFunc;
    return n;
}

Node *createList(int value, Node *nextNode) {
    Node *n = allocNode();
    n->type = LIST;
    n->data.listLiteral = value;
    n->right = nextNode;
    return n;
}

Node *createEnvFrame(int size, Node *parentScope) {
    Node *n = allocNode();
    n->type = ENV_FRAME;
    n->errorFlags = size;
    setLeft(n, parentScope);

    n->data.locals = malloc(sizeof(Node *) * size);
    for (int i = 0; i < size; i++) {
        n->data.locals[i] = NULL;
    }

    return n;
}

Node *copyTree(Node *root) {
    if (root == NULL)
        return NULL;
    Node *clone = allocNode();
    pushRoot(clone);
    clone->type = root->type;
    clone->data = root->data;

    clone->left = copyTree(root->left);
    clone->right = copyTree(root->right);
    popRoot();

    return clone;
}

Node *substitute(Node *root, char *paramName, Node *argValue) {
    if (root == NULL)
        return NULL;

    if (root->type == GLOBAL_VAR && strcmp(root->data.var, paramName) == 0) {
        return argValue;
    }

    root->left = substitute(root->left, paramName, argValue);
    root->right = substitute(root->right, paramName, argValue);

    return root;
}

Node *createArgs1(Node *arg1) {
    Node *l1 = createList(0, NULL);
    l1->left = arg1;
    return l1;
}

Node *createArgs2(Node *arg1, Node *arg2) {
    Node *l2 = createList(0, NULL);
    l2->left = arg2;
    Node *l1 = createList(0, l2);
    l1->left = arg1;
    return l1;
}

Node *createArgs3(Node *arg1, Node *arg2, Node *arg3) {
    Node *l3 = createList(0, NULL);
    l3->left = arg3;
    Node *l2 = createList(0, l3);
    l2->left = arg2;
    Node *l1 = createList(0, l2);
    l1->left = arg1;
    return l1;
}

Node *createUserData(int typeID, int subType, Node *left, Node *right,
                     void *rawData) {
    Node *n = allocNode();
    n->type = USER_DATA;
    n->infoFlags = typeID;
    n->errorFlags = subType;
    n->data.userdata = rawData;
    n->left = left;
    n->right = right;
    return n;
}

void *getUserData(Node *n) { return n->data.userdata; }
int getTypeID(Node *n) { return n->infoFlags; }
int getSubType(Node *n) { return n->errorFlags; }

NodeType getNodeType(Node *n) { return n ? n->type : (NodeType)-1; }
Node *getLeft(Node *n) { return n ? n->left : NULL; }
Node *getRight(Node *n) { return n ? n->right : NULL; }
int getLiteral(Node *n) { return n ? n->data.literal : 0; }
char *getVarName(Node *n) { return n ? n->data.var : NULL; }
char *getFuncName(Node *n) { return n ? n->data.call : NULL; }

int getVarIndex(Node *n) { return n ? n->data.index : 0; }
int getVarDepth(Node *n) { return n ? n->infoFlags : 0; }
Node **getLocalsArray(Node *n) { return n ? n->data.locals : NULL; }
int getLocalsCount(Node *n) { return n ? n->errorFlags : 0; }

void setLeft(Node *n, Node *left) {
    if (n)
        n->left = left;
}
void setRight(Node *n, Node *right) {
    if (n)
        n->right = right;
}
Node *getCapturedEnv(Node *n) { return n ? n->data.closure : NULL; }
