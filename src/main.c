#include "Allocator.h"
#include "GarbageCollector.h"
#include "Parser.h"
#include "PluginAPI.h"
#include "PluginAPIHandle.h"
#include "Evaluator.h"
#include <dlfcn.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TYPE_IO 100
#define IO_RETURN 0
#define IO_PRINT 1
#define IO_BIND 2
#define IO_SEQ 3

jmp_buf error_jmp;

Node *executeIO(Node *action) {
    while (action != NULL && getNodeType(action) == USER_DATA && getTypeID(action) == TYPE_IO) {
        int subType = getSubType(action);
        
        if (subType == IO_RETURN) {
            return getLeft(action);
        } else if (subType == IO_PRINT) {
            Node *val = getLeft(action);
            if (getNodeType(val) == LITERAL) {
                printf("%d\n", getLiteral(val));
            } else {
                printf("<non-literal output>\n");
            }
            return NULL; // unit
        } else if (subType == IO_BIND) {
            Node *firstAction = getLeft(action);
            Node *callback = getRight(action);

            Node *res1 = executeIO(firstAction);
            
            // Apply callback to res1
            Node *args = createList(0, NULL);
            setLeft(args, res1);
            Node *call = createFunction(callback, args);
            
            pushRoot(call);
            action = evaluate(call, NULL); // evaluate returns the next IO action!
            popRoot();
        } else if (subType == IO_SEQ) {
            Node *firstAction = getLeft(action);
            Node *secondAction = getRight(action);

            executeIO(firstAction);
            action = secondAction; // tail recurse into the second action
        } else {
            break;
        }
    }
    return action;
}

void loadPlugin(const char *path, VMAPI api) {
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        printf("FFI Error: Failed to load plugin '%s'\nReason: %s\n", path,
               dlerror());
        return;
    }
    dlerror();
    void (*initPlugin)(VMAPI);
    *(void **)(&initPlugin) = dlsym(handle, "initPlugin");
    const char *err = dlerror();
    if (err != NULL) {
        printf(
            "FFI Error: Could not find 'initPlugin' inside '%s'\nReason: %s\n",
            path, err);
        dlclose(handle);
        return;
    }
    initPlugin(api);
    printf("Successfully loaded plugin: %s\n", path);
}

int main(void) {
    VMAPI api = getHandleAPI();

    // hardcoded for now
    loadPlugin("./out/CoreMath.so", api);
    loadPlugin("./out/CoreIO.so", api);

    printf("Starting GraphLang VM...\n");

    initAllocator();
    enableGC();

    printf("\n\033[1;36m==============================\033[0m\n");
    printf("\033[1;32m   GraphLang Interactive REPL   \033[0m\n");
    printf("\033[1;36m==============================\033[0m\n");
    printf("Type your Lisp expressions below. (Ctrl+C to exit)\n\n");

    char buffer[4096] = {0};
    int openParens = 0;
    int isMultiline = 0;

    rl_variable_bind("blink-matching-paren", "on");

    while (1) {
        char *input = NULL;

        if (!isMultiline) {
            input = readline("\001\033[1;33m\002λ > \001\033[0m\002");
            buffer[0] = '\0';
        } else {
            char promptBuf[256];
            strcpy(promptBuf, "\001\033[1;33m\002  > \001\033[0m\002");
            for (int i = 0; i < openParens; i++) {
                strcat(promptBuf,
                       "    "); // Readline handles spaces better than tabs
            }
            input = readline(promptBuf);
        }

        if (!input) {
            break;
        }

        // Empty line
        if (input[0] == '\0') {
            if (isMultiline) {
                if (buffer[0] != '\0') {
                    if (setjmp(error_jmp) == 0) {
                        parse(buffer);
                    }
                }
                isMultiline = 0;
                openParens = 0;
            }
            free(input);
            continue;
        }

        add_history(input);
        isMultiline = 1;

        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == ';')
                break;
            if (input[i] == '(')
                openParens++;
            if (input[i] == ')')
                openParens--;
        }

        if (openParens < 0)
            openParens = 0;

        strncat(buffer, input, sizeof(buffer) - strlen(buffer) - 2);
        strcat(buffer, "\n"); // Add the newline back
        free(input);
    }
    printf("VM Shutdown safely.\n");

    return 0;
}
