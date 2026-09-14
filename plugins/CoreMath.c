#include "PluginAPI.h"
#include <stddef.h>

static VMAPI vm;

Node *nativeAdd(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) + vm.getLiteral(r));
    vm.popRoot();
    return res;
}

Node *nativeSub(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) - vm.getLiteral(r));
    vm.popRoot();
    return res;
}

Node *nativeMul(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) * vm.getLiteral(r));
    vm.popRoot();
    return res;
}

Node *nativeDiv(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) / vm.getLiteral(r));
    vm.popRoot();
    return res;
}

Node *nativeLessThan(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) < vm.getLiteral(r) ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeGreaterThan(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) > vm.getLiteral(r) ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeEquals(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(l) == vm.getLiteral(r) ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeAnd(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    if (vm.getLiteral(l) == 0) {
        Node *res = vm.createLiteral(0);
        vm.popRoot();
        return res;
    }
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(r) != 0 ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeOr(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *right = vm.getLeft(vm.getRight(args));
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    if (vm.getLiteral(l) != 0) {
        Node *res = vm.createLiteral(1);
        vm.popRoot();
        return res;
    }
    Node *r = vm.evaluate(right, env);
    Node *res = vm.createLiteral(vm.getLiteral(r) != 0 ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeNot(Node *args, struct LocalEnv *env) {
    Node *left = vm.getLeft(args);
    Node *l = vm.evaluate(left, env);
    vm.pushRoot(l);
    Node *res = vm.createLiteral(vm.getLiteral(l) == 0 ? 1 : 0);
    vm.popRoot();
    return res;
}

Node *nativeIf(Node *args, struct LocalEnv *env) {
    Node *condition = vm.getLeft(args);
    Node *trueBranch = vm.getLeft(vm.getRight(args));
    Node *falseBranch = vm.getLeft(vm.getRight(vm.getRight(args)));

    Node *cond = vm.evaluate(condition, env);
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
