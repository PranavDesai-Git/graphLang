#include "PluginAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TYPE_STRING;
static VMAPI vm;

Handle nativeFileRead(Handle args, Handle env) {
    char *filename;
    if (!vm.unpackArgs(args, env, "s", &filename)) return vm.createLiteral(0);

    FILE *f = fopen(filename, "rb");
    if (!f) return vm.createLiteral(0);

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, f);
    fclose(f);
    buffer[fsize] = 0;

    return vm.createForeign(TYPE_STRING, 0, -1, -1, buffer);
}

Handle nativeFileWrite(Handle args, Handle env) {
    char *filename;
    char *content;
    if (!vm.unpackArgs(args, env, "ss", &filename, &content)) return vm.createLiteral(0);

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
