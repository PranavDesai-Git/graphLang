#include "Environment.h"
#include "TreeNode.h"
#include "GarbageCollector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *evaluate(Node *node, LocalEnv *env) {
    if (node == NULL)
        return NULL;

    switch (getNodeType(node)) {
    case LITERAL:
        return node;

    case VARIABLE: {
        LocalEnv *temp = env;
        char *varName = getVarName(node);
        while (temp != NULL) {
            if (strcmp(temp->varName, varName) == 0) {
                Node *evaluatedArg = evaluate(temp->value, env);
                temp->value = evaluatedArg; // Memoize in the environment!
                return evaluatedArg;
            }
            temp = temp->next;
        }
        EnvEntry *var = getEnvEntry(varName);
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
        if (getNodeType(funcNode) != VARIABLE) {
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
            LocalEnv newEnv;
            newEnv.varName = getVarName(getLeft(func->params));
            newEnv.value = evaluate(getLeft(getRight(node)), env);
            pushRoot(newEnv.value); // PROTECT FROM GC!
            newEnv.next = env;
            result = evaluate(func->val.node, &newEnv);
            popRoot(); // UNPROTECT
        } else {
            printf("Runtime Error: '%s' is not a function!\n", func->key);
            exit(1);
        }

        return result;
    }

    case LIST:
    default:
        return node;
    }
}
