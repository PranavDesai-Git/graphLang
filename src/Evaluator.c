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
        return var->val;
    }

    case CALL: { 
        Node *funcNode = evaluate(getLeft(node), env); 
        pushRoot(funcNode);
        Node *result = NULL;
        
        if (getNodeType(funcNode) == FOREIGN) { 
            int scope = rootCount; 
            pushRoot(getRight(node));
            Handle argsH = rootCount - 1;
            pushRoot(env);
            Handle envH = rootCount - 1;

            Handle (*plugin_func)(Handle, Handle) = (Handle (*)(Handle, Handle))getUserData(funcNode);
            Handle resH = plugin_func(argsH, envH);

            result = gcRoots[resH];
            rootCount = scope; 
        } else if (getNodeType(funcNode) == CLOSURE) { 
            int argCount = 0;
            Node *temp = getRight(node);
            while (temp != NULL && getNodeType(temp) == CONS) {
                argCount++;
                temp = getRight(temp);
            }

            Node *newEnv = createEnvFrame(argCount, getCapturedEnv(funcNode));
            pushRoot(newEnv); 

            temp = getRight(node);
            int i = 0;
            while (temp != NULL && getNodeType(temp) == CONS) {
                Node *argValue = evaluate(getLeft(temp), env);
                getLocalsArray(newEnv)[i++] = argValue;
                temp = getRight(temp);
            }

            result = evaluate(getRight(funcNode), newEnv);
            popRoot(); 
        } else {
            printf("Runtime Error: Not a function!\n");
            longjmp(error_jmp, 1);
        }

        popRoot(); // funcNode
        return result;
    }

    case CONS:
    case ENV_FRAME:
    default:
        return node;

    case LAMBDA: {
        Node *closure = createClosure(getLeft(node), getRight(node), env);
        return closure;
    }
    
    case DEFINE: {
        Node *value = evaluate(getLeft(node), env);
        defineVariable(getVarName(node), value);
        return value;
    }
    }
}
