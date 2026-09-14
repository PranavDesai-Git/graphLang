#include "Allocator.h"
#include "Parser.h"
#include "PluginAPI.h"
#include "PluginAPIHandle.h"
#include <dlfcn.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <setjmp.h>

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

    while (1) {
        if (!isMultiline) {
            printf("\033[1;33mλ > \033[0m");
            buffer[0] = '\0';
        } else {
            printf("\033[1;33m  > \033[0m"); 
            for (int i = 0; i < openParens; i++) {
                printf("\t");
            }
        }

        char line[1024];
        if (!fgets(line, sizeof(line), stdin)) {
            break; 
        }

        if (line[0] == '\n') {
            if (isMultiline) {
                if (buffer[0] != '\0') {
                    if (setjmp(error_jmp) == 0) {
                        parse(buffer);
                    }
                }
                isMultiline = 0;
                openParens = 0;
            }
            continue;
        }

        isMultiline = 1;

        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ';') break; 
            if (line[i] == '(') openParens++;
            if (line[i] == ')') openParens--;
        }

        if (openParens < 0) openParens = 0;

        strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    }
    printf("VM Shutdown safely.\n");

    return 0;
}
