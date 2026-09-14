#include "PluginAPI.h"
#include <stddef.h>

#define TYPE_IO 100
#define IO_RETURN 0
#define IO_PRINT 1
#define IO_BIND 2

static VMAPI vm;

Handle nativeIOReturn(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle evalVal = vm.evaluate(val, env);
    return vm.createUserData(TYPE_IO, IO_RETURN, evalVal, 0, NULL);
}

Handle nativeIOPrint(Handle args, Handle env) {
    Handle val = vm.getLeft(args);
    Handle evalVal = vm.evaluate(val, env);
    return vm.createUserData(TYPE_IO, IO_PRINT, evalVal, 0, NULL);
}

Handle nativeIOBind(Handle args, Handle env) {
    Handle action = vm.getLeft(args);
    Handle callback = vm.getLeft(vm.getRight(args));
    
    // Evaluate the action and callback nodes themselves
    Handle evalAction = vm.evaluate(action, env);
    Handle evalCallback = vm.evaluate(callback, env);
    
    return vm.createUserData(TYPE_IO, IO_BIND, evalAction, evalCallback, NULL);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    vm.registerNative("io.return", nativeIOReturn);
    vm.registerNative("io.print", nativeIOPrint);
    vm.registerNative("io.bind", nativeIOBind);
}
