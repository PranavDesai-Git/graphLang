#include "PluginAPI.h"
#include <stddef.h>

#define IO_RETURN 0
#define IO_PRINT 1
#define IO_BIND 2
#define IO_SEQ 3
#define IO_PRINT_STR 4

static int TYPE_IO;
static VMAPI vm;

Handle nativeIOReturn(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle evalVal = vm.evaluate(val, env);
    return vm.createForeign(TYPE_IO, IO_RETURN, evalVal, 0, NULL);
}

Handle nativeIOPrint(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle evalVal = vm.evaluate(val, env);
    return vm.createForeign(TYPE_IO, IO_PRINT, evalVal, 0, NULL);
}

Handle nativeIOBind(Handle args, Handle env) {
    Handle action = vm.getLeft(args);
    Handle callback = vm.getLeft(vm.getRight(args));

    // Evaluate the action and callback nodes themselves
    Handle evalAction = vm.evaluate(action, env);
    Handle evalCallback = vm.evaluate(callback, env);

    return vm.createForeign(TYPE_IO, IO_BIND, evalAction, evalCallback, NULL);
}

Handle nativeIOSeq(Handle args, Handle env) {
    Handle action1 = vm.getLeft(args);
    Handle action2 = vm.getLeft(vm.getRight(args));

    Handle evalAction1 = vm.evaluate(action1, env);
    Handle evalAction2 = vm.evaluate(action2, env);

    return vm.createForeign(TYPE_IO, IO_SEQ, evalAction1, evalAction2, NULL);
}

Handle nativeIOPrintStr(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle evalVal = vm.evaluate(val, env);
    return vm.createForeign(TYPE_IO, IO_PRINT_STR, evalVal, 0, NULL);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    TYPE_IO = vm.registerType("IO");
    vm.registerNative("io.return", nativeIOReturn);
    vm.registerNative("io.print", nativeIOPrint);
    vm.registerNative("io.print_str", nativeIOPrintStr);
    vm.registerNative("io.bind", nativeIOBind);
    vm.registerNative("io.seq", nativeIOSeq);
}
