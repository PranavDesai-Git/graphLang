#include "Allocator.h"
#include "Evaluator.h"
#include "GarbageCollector.h"
#include "Parser.h"
#include "PluginAPI.h"
#include "PluginAPIHandle.h"
#include "TreeNode.h"
#include <dlfcn.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IO_RETURN 0
#define IO_PRINT 1
#define IO_BIND 2
#define IO_SEQ 3

int TYPE_IO; // Populated during initialization

jmp_buf error_jmp;

Node *executeIO(Node *action) {
    while (action != NULL && getNodeType(action) == FOREIGN &&
           getTypeID(action) == TYPE_IO) {
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
            Node *args = createCons(0, NULL);
            setLeft(args, res1);
            Node *call = createCall(callback, args);

            pushRoot(call);
            action =
                evaluate(call, NULL); // evaluate returns the next IO action!
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
}

#include <dirent.h>

void loadAllPlugins(const char *dirPath, VMAPI api) {
    DIR *dir = opendir(dirPath);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 3 && strcmp(entry->d_name + len - 3, ".so") == 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);
            loadPlugin(path, api);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    VMAPI api = getHandleAPI();

    // Register our IO Type dynamically before plugins load
    TYPE_IO = api_registerType("IO");

    initEnvironment();

    // Pre-load plugins
    loadAllPlugins("./out", api);

    initAllocator();
    enableGC();

    if (argc > 1) {
        FILE *file = fopen(argv[1], "rb");
        if (file == NULL) {
            printf("Error: Could not open file '%s'\n", argv[1]);
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);

        char *buffer = malloc(fileSize + 1);
        if (buffer == NULL) {
            printf("Error: Not enough memory to read file '%s'\n", argv[1]);
            fclose(file);
            return 1;
        }

        size_t bytesRead = fread(buffer, 1, fileSize, file);
        buffer[bytesRead] = '\0';
        fclose(file);

        if (setjmp(error_jmp) == 0) {
            parse(buffer);
        } else {
            printf("Caught parse error in file: %s\n", argv[1]);
        }
        
        free(buffer);
        return 0;
    }

    printf("Starting GraphLang VM...\n");
    printf("\n\033[1;36m==============================\033[0m\n");
    printf("\033[1;32m   GraphLang Interactive REPL   \033[0m\n");
    printf("\033[1;36m==============================\033[0m\n");
    printf("Type your Lisp expressions below. (Ctrl+C to exit)\n\n");

    char *buffer = NULL;
    size_t bufferSize = 0;
    size_t bufferLen = 0;
    int openParens = 0;
    int isMultiline = 0;

    rl_variable_bind("blink-matching-paren", "on");

    while (1) {
        char *input = NULL;

        if (!isMultiline) {
            input = readline("\001\033[1;33m\002λ > \001\033[0m\002");
            bufferLen = 0;
            if (buffer) buffer[0] = '\0';
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
                if (buffer && buffer[0] != '\0') {
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

        size_t inputLen = strlen(input);
        if (bufferLen + inputLen + 2 > bufferSize) {
            bufferSize = bufferSize == 0 ? 4096 : bufferSize * 2 + inputLen + 2;
            buffer = realloc(buffer, bufferSize);
            if (!buffer) {
                printf("Error: Out of memory\n");
                exit(1);
            }
        }

        strcpy(buffer + bufferLen, input);
        bufferLen += inputLen;
        buffer[bufferLen++] = '\n';
        buffer[bufferLen] = '\0';

        free(input);
    }
    if (buffer) free(buffer);
    printf("VM Shutdown safely.\n");

    return 0;
}
