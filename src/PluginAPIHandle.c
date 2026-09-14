#include "PluginAPIHandle.h"
#include "Environment.h"
#include "Evaluator.h"
#include "GarbageCollector.h"
#include "TreeNode.h"
#include <stdio.h>
#include <stdlib.h>

// Helper to push a raw Node to the GC root stack and return its index
static Handle makeHandle(Node *n) {
    if (n == NULL)
        return -1;
    pushRoot(n);
    return rootCount - 1;
}

// Helper to resolve an index back to a raw Node pointer
static Node *resolveHandle(Handle h) {
    if (h < 0 || h >= rootCount)
        return NULL;
    return gcRoots[h];
}

// ==========================================
// API WRAPPERS
// ==========================================

static void api_registerNative(char *name,
                               Handle (*func)(Handle args, Handle env)) {
    // We cast the handle function back to the internal raw Node pointer
    // function type. The Evaluator handles this gracefully when it unpacks the
    // plugin return value.
    registerNative(name, (Func)func);
}

static Handle api_evaluate(Handle node, Handle env) {
    return makeHandle(evaluate(resolveHandle(node), resolveHandle(env)));
}

static Handle api_createLiteral(int value) {
    return makeHandle(createLiteral(value));
}

static Handle api_createGlobalVar(char *varName) {
    return makeHandle(createGlobalVar(varName));
}

static Handle api_createLocalVar(int depth, int index) {
    return makeHandle(createLocalVar(depth, index));
}

static Handle api_createFunction(Handle funcExpr, Handle args) {
    return makeHandle(
        createFunction(resolveHandle(funcExpr), resolveHandle(args)));
}

static Handle api_createList(int value, Handle nextNode) {
    return makeHandle(createList(value, resolveHandle(nextNode)));
}

static Handle api_copyTree(Handle root) {
    return makeHandle(copyTree(resolveHandle(root)));
}

static void api_pushRoot(Handle node) { pushRoot(resolveHandle(node)); }

static void api_popRoot(void) { popRoot(); }

static NodeType api_getNodeType(Handle n) {
    return getNodeType(resolveHandle(n));
}

static Handle api_getLeft(Handle n) {
    return makeHandle(getLeft(resolveHandle(n)));
}

static Handle api_getRight(Handle n) {
    return makeHandle(getRight(resolveHandle(n)));
}

static int api_getLiteral(Handle n) { return getLiteral(resolveHandle(n)); }

static char *api_getVarName(Handle n) { return getVarName(resolveHandle(n)); }

static char *api_getFuncName(Handle n) { return getFuncName(resolveHandle(n)); }

static Handle api_createUserData(int typeID, int subType, Handle leftH,
                                 Handle rightH, void *rawData) {
    Node *left = (leftH != 0) ? gcRoots[leftH] : NULL;
    Node *right = (rightH != 0) ? gcRoots[rightH] : NULL;
    Node *res = createUserData(typeID, subType, left, right, rawData);
    pushRoot(res);
    return rootCount - 1;
}
static void *api_getUserData(Handle n) { return getUserData(gcRoots[n]); }
static int api_getTypeID(Handle n) { return getTypeID(gcRoots[n]); }
static int api_getSubType(Handle n) { return getSubType(gcRoots[n]); }

// Return the fully wrapped API Struct
VMAPI getHandleAPI(void) {
    VMAPI api = {.registerNative = api_registerNative,
                 .evaluate = api_evaluate,
                 .createLiteral = api_createLiteral,
                 .createGlobalVar = api_createGlobalVar,
                 .createLocalVar = api_createLocalVar,
                 .createFunction = api_createFunction,
                 .createList = api_createList,
                 .copyTree = api_copyTree,
                 .pushRoot = api_pushRoot,
                 .popRoot = api_popRoot,
                 .getNodeType = api_getNodeType,
                 .getLeft = api_getLeft,
                 .getRight = api_getRight,
                 .getLiteral = api_getLiteral,
                 .getVarName = api_getVarName,
                 .getFuncName = api_getFuncName,
                 .createUserData = api_createUserData,
                 .getUserData = api_getUserData,
                 .getTypeID = api_getTypeID,
                 .getSubType = api_getSubType};
    return api;
}
