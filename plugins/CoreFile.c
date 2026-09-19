#include "PluginAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TYPE_STRING;
static VMAPI vm;

Handle nativeFileRead(Handle args, Handle env) {
    Handle fileH = vm.getLeft(args);
    Handle l = vm.evaluate(fileH, env);
    char *filename = (char*)vm.getUserData(l);
    
    if (filename == NULL) return vm.createForeign(TYPE_STRING, 0, -1, -1, NULL);
    
    FILE *f = fopen(filename, "rb");
    if (!f) return vm.createForeign(TYPE_STRING, 0, -1, -1, NULL);
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    
    string[fsize] = 0;
    
    return vm.createForeign(TYPE_STRING, 0, -1, -1, string);
}

Handle nativeFileWrite(Handle args, Handle env) {
    Handle fileH = vm.getLeft(args);
    Handle contentH = vm.getLeft(vm.getRight(args));
    
    Handle l = vm.evaluate(fileH, env);
    Handle r = vm.evaluate(contentH, env);
    
    char *filename = (char*)vm.getUserData(l);
    char *content = (char*)vm.getUserData(r);
    
    if (filename == NULL || content == NULL) return vm.createLiteral(0);
    
    FILE *f = fopen(filename, "wb");
    if (!f) return vm.createLiteral(0);
    
    size_t written = fwrite(content, 1, strlen(content), f);
    fclose(f);
    
    return vm.createLiteral(written == strlen(content) ? 1 : 0);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    TYPE_STRING = vm.registerType("String");
    vm.registerNative("file.read", nativeFileRead);
    vm.registerNative("file.write", nativeFileWrite);
}
