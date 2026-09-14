#include "Allocator.h"
#include "Parser.h"
#include "PluginAPI.h"
#include "PluginAPIHandle.h"
#include <dlfcn.h>
#include <stdio.h>
#include <time.h>

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

    parse("(define customAdd (a b) (+ a b)) (customAdd 100 500)");
    printf("VM Shutdown safely.\n");

    return 0;
}
