#include "Allocator.h"
#include "Parser.h"
#include "PluginAPI.h"
#include "PluginAPIHandle.h"
#include <dlfcn.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <setjmp.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

jmp_buf error_jmp;

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

    printf("Starting GraphLang VM...\n");

    initAllocator();

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
                strcat(promptBuf, "    "); // Readline handles spaces better than tabs
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
            if (input[i] == ';') break; 
            if (input[i] == '(') openParens++;
            if (input[i] == ')') openParens--;
        }

        if (openParens < 0) openParens = 0;

        strncat(buffer, input, sizeof(buffer) - strlen(buffer) - 2);
        strcat(buffer, "\n"); // Add the newline back
        free(input);
    }
    printf("VM Shutdown safely.\n");

    return 0;
}
