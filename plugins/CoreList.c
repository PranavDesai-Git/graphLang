#include "PluginAPI.h"
#include <stddef.h>

static VMAPI vm;

Handle nativeCons(Handle args, Handle env) {
    Handle leftExpr = vm.getLeft(args);
    Handle rightExpr = vm.getLeft(vm.getRight(args));

    Handle leftVal = vm.evaluate(leftExpr, env);
    Handle rightVal = vm.evaluate(rightExpr, env);

    Handle consNode = vm.createCons(0, rightVal);
    vm.setLeft(consNode, leftVal);
    return consNode;
}

Handle nativeCar(Handle args, Handle env) {
    Handle listH = vm.getLeft(args);
    Handle l = vm.evaluate(listH, env);
    return vm.getLeft(l);
}

Handle nativeCdr(Handle args, Handle env) {
    Handle listH = vm.getLeft(args);
    Handle l = vm.evaluate(listH, env);
    return vm.getRight(l);
}

Handle nativeIsNull(Handle args, Handle env) {
    Handle listH = vm.getLeft(args);
    Handle l = vm.evaluate(listH, env);
    
    if (l < 0 || vm.getNodeType(l) == (NodeType)-1) {
        return vm.createLiteral(1);
    }
    return vm.createLiteral(0);
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    vm.registerNative("cons", nativeCons);
    vm.registerNative("car", nativeCar);
    vm.registerNative("cdr", nativeCdr);
    vm.registerNative("is_null", nativeIsNull);
}
