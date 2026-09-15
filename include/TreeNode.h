#ifndef TREENODE_H
#define TREENODE_H

typedef enum {
    LITERAL,
    GLOBAL_VAR,
    LOCAL_VAR,
    CALL,
    LIST,
    ENV_FRAME,
    USER_DATA,
    CLOSURE,
    NATIVE_FUNC
} NodeType;

typedef struct Node Node;

/*
struct Node *left;
struct Node *right;

union {
    int literal;
    char *var;
    char *func;
    int listLiteral;
} data;

NodeType type;

unsigned int infoFlags;
unsigned int errorFlags;
unsigned int statusFlags;
*/

NodeType getNodeType(Node *n);
Node *getLeft(Node *n);
Node *getRight(Node *n);
int getLiteral(Node *n);
char *getVarName(Node *n);
char *getFuncName(Node *n);

int getVarIndex(Node *n);
int getVarDepth(Node *n);
Node **getLocalsArray(Node *n);
int getLocalsCount(Node *n);

void setLeft(Node *n, Node *left);
void setRight(Node *n, Node *right);

Node *createLiteral(int value);
Node *createGlobalVar(char *varName);
Node *createLocalVar(int depth, int index);
Node *createCall(Node *funcExpr, Node *args);
Node *createClosure(Node *params, Node *body, Node *env);
Node *createNativeFunc(void *cFunc);
Node *createList(int value, Node *nextNode);
Node *createEnvFrame(int size, Node *parentScope);
Node *createArgs1(Node *arg1);
Node *createArgs2(Node *arg1, Node *arg2);
Node *createArgs3(Node *arg1, Node *arg2, Node *arg3);
Node *getCapturedEnv(Node *n);

Node *createUserData(int typeID, int subType, Node *left, Node *right,
                     void *rawData);
void *getUserData(Node *n);
int getTypeID(Node *n);
int getSubType(Node *n);

#endif // TREENODE_H
