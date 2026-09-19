#include "PluginAPI.h"
#include <stdio.h>
#include <stdlib.h>

static int TYPE_STRING;
static VMAPI vm;

Handle nativeImport(Handle args, Handle env) {
    char *filename;
    if (!vm.unpackArgs(args, env, "s", &filename)) return vm.createLiteral(0);
    
    if (filename != NULL) {
        vm.runFile(filename);
    }
    
    return vm.createLiteral(1);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    TYPE_STRING = vm.registerType("String");
    vm.registerNative("import", nativeImport);
}
