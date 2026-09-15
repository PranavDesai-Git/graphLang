#include "PluginAPI.h"
#include <stddef.h>
#include <stdio.h>

static VMAPI vm;

Handle nativeAdd(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));

    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);

    int l_val = vm.getLiteral(l);
    int r_val = vm.getLiteral(r);

    return vm.createLiteral(l_val + r_val);
}

Handle nativeSub(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) - vm.getLiteral(r));
}

Handle nativeMul(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) * vm.getLiteral(r));
}

Handle nativeDiv(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) / vm.getLiteral(r));
}

Handle nativeLessThan(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) < vm.getLiteral(r) ? 1 : 0);
}

Handle nativeGreaterThan(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) > vm.getLiteral(r) ? 1 : 0);
}

Handle nativeEquals(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(l) == vm.getLiteral(r) ? 1 : 0);
}

Handle nativeAnd(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    if (vm.getLiteral(l) == 0) {
        return vm.createLiteral(0);
    }
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(r) != 0 ? 1 : 0);
}

Handle nativeOr(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle right = vm.getLeft(vm.getRight(args));
    Handle l = vm.evaluate(left, env);
    if (vm.getLiteral(l) != 0) {
        return vm.createLiteral(1);
    }
    Handle r = vm.evaluate(right, env);
    return vm.createLiteral(vm.getLiteral(r) != 0 ? 1 : 0);
}

Handle nativeNot(Handle args, Handle env) {
    Handle left = vm.getLeft(args);
    Handle l = vm.evaluate(left, env);
    return vm.createLiteral(vm.getLiteral(l) == 0 ? 1 : 0);
}

Handle nativeIf(Handle args, Handle env) {
    Handle condition = vm.getLeft(args);
    Handle trueBranch = vm.getLeft(vm.getRight(args));
    Handle falseBranch = vm.getLeft(vm.getRight(vm.getRight(args)));

    Handle cond = vm.evaluate(condition, env);
    if (vm.getLiteral(cond) == 1) {
        return vm.evaluate(trueBranch, env);
    } else {
        return vm.evaluate(falseBranch, env);
    }
}

void initPlugin(VMAPI providedApi) {
    vm = providedApi;
    vm.registerNative("+", nativeAdd);
    vm.registerNative("-", nativeSub);
    vm.registerNative("*", nativeMul);
    vm.registerNative("/", nativeDiv);
    vm.registerNative("<", nativeLessThan);
    vm.registerNative(">", nativeGreaterThan);
    vm.registerNative("==", nativeEquals);
    vm.registerNative("&&", nativeAnd);
    vm.registerNative("||", nativeOr);
    vm.registerNative("!", nativeNot);
    vm.registerNative("?", nativeIf);
}
