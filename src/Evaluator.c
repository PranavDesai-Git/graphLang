#include "Environment.h"
#include "GarbageCollector.h"
#include "TreeNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
extern jmp_buf error_jmp;
#include "PluginAPI.h"

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
            longjmp(error_jmp, 1);
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
            longjmp(error_jmp, 1);
        }

        EnvEntry *func = getEnvEntry(getVarName(funcNode));
        if (func == NULL) {
            printf("Runtime Error: Undefined function '%s'\n",
                   getVarName(funcNode));
            longjmp(error_jmp, 1);
        }

        Node *result;

        if (func->isFunc == 1) { // Native C Function
            int scope = rootCount; // Open Handle Scope
            pushRoot(getRight(node));
            Handle argsH = rootCount - 1;
            pushRoot(env);
            Handle envH = rootCount - 1;

            Handle (*plugin_func)(Handle, Handle) = (Handle (*)(Handle, Handle))func->val.func;
            Handle resH = plugin_func(argsH, envH);

            result = gcRoots[resH];
            rootCount = scope; // Close Handle Scope
        } else if (func->isFunc == 2) { // User-Defined Function
            // Count arguments
            int argCount = 0;
            Node *temp = getRight(node);
            while (temp != NULL && getNodeType(temp) == LIST) {
                argCount++;
                temp = getRight(temp);
            }

            // Create environment frame and protect it immediately
            Node *newEnv = createEnvFrame(argCount, env);
            pushRoot(newEnv); 

            // Evaluate arguments and store them
            temp = getRight(node);
            int i = 0;
            while (temp != NULL && getNodeType(temp) == LIST) {
                Node *argValue = evaluate(getLeft(temp), env);
                getLocalsArray(newEnv)[i++] = argValue;
                temp = getRight(temp);
            }

            result = evaluate(func->val.node, newEnv);
            popRoot(); // UNPROTECT env
        } else {
            printf("Runtime Error: '%s' is not a function!\n", func->key);
            longjmp(error_jmp, 1);
        }

        return result;
    }

    case LIST:
    case ENV_FRAME:
    default:
        return node;
    }
}
