#include "PluginAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TYPE_STRING;
static VMAPI vm;

Handle nativeStrMake(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle l = vm.evaluate(val, env);
    int num = vm.getLiteral(l);
    char *str = malloc(32);
    snprintf(str, 32, "%d", num);
    return vm.createForeign(TYPE_STRING, 0, -1, -1, str);
}

Handle nativeStrConcat(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    
    char *s1 = (char*)vm.getUserData(l);
    char *s2 = (char*)vm.getUserData(r);
    
    if (s1 == NULL) s1 = "";
    if (s2 == NULL) s2 = "";
    
    char *newStr = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(newStr, s1);
    strcat(newStr, s2);
    
    return vm.createForeign(TYPE_STRING, 0, -1, -1, newStr);
}

Handle nativeStrLen(Handle args, Handle env) {
    Handle strH = vm.getLeft(args);
    Handle s = vm.evaluate(strH, env);
    
    char *str = (char*)vm.getUserData(s);
    if (str == NULL) return vm.createLiteral(0);
    
    return vm.createLiteral(strlen(str));
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    TYPE_STRING = vm.registerType("String");
    vm.registerNative("str.make", nativeStrMake);
    vm.registerNative("str.concat", nativeStrConcat);
    vm.registerNative("str.len", nativeStrLen);
}
