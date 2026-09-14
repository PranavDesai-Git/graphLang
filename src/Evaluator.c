#include "Environment.h"
#include "GarbageCollector.h"
#include "TreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *evaluate(Node *node, Node *env) {
    if (node == NULL)
        return NULL;

    switch (getNodeType(node)) {
    case LITERAL:
        return node;

    case LOCAL_VAR: {
        Node *tempEnv = env;
        int depth = getVarDepth(node);
        for (int i = 0; i < depth; i++) {
            tempEnv = getLeft(tempEnv);
        }
        int idx = getVarIndex(node);
        return getLocalsArray(tempEnv)[idx];
    }
    
    case GLOBAL_VAR: {
        EnvEntry *var = getEnvEntry(getVarName(node));
        if (var == NULL) {
            printf("Runtime Error: Undefined variable '%s'\n", getVarName(node));
            exit(1);
        }
        if (var->isFunc == 1 || var->isFunc == 2) {
            return node; // Return the function identifier node as-is!
        }
        return evaluate(var->val.node, env);
    }

    case FUNCTION: { // This is an APPLICATION node
        Node *funcNode = evaluate(getLeft(node), env); // Get the function to run
        if (getNodeType(funcNode) != GLOBAL_VAR) {
            printf("Runtime Error: Not a function!\n");
            exit(1);
        }

        EnvEntry *func = getEnvEntry(getVarName(funcNode));
        if (func == NULL) {
            printf("Runtime Error: Undefined function '%s'\n",
                   getVarName(funcNode));
            exit(1);
        }

        Node *result;

        if (func->isFunc == 1) { // Native C Function
            Func eval = func->val.func;
            result = eval(getRight(node), env); // Args are in right

        } else if (func->isFunc == 2) { // User-Defined Function
            Node *argValue = evaluate(getLeft(getRight(node)), env);
            pushRoot(argValue); // PROTECT FROM GC!
            Node *newEnv = createEnvFrame(1, env);
            getLocalsArray(newEnv)[0] = argValue;
            popRoot(); // UNPROTECT arg
            pushRoot(newEnv);
            result = evaluate(func->val.node, newEnv);
            popRoot(); // UNPROTECT env
        } else {
            printf("Runtime Error: '%s' is not a function!\n", func->key);
            exit(1);
        }

        return result;
    }

    case LIST:
    case ENV_FRAME:
    default:
        return node;
    }
}
