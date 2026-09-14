#include "Allocator.h"
#include "Environment.h"
#include "Evaluator.h"
#include "GarbageCollector.h"
#include "PluginAPI.h"
#include "TreeNode.h"
#include <dlfcn.h>
#include <stdio.h>
#include <time.h>

void loadPlugin(const char *path, VMAPI api) {
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        printf("FFI Error: Failed to load plugin '%s'\nReason: %s\n", path,
               dlerror());
        return;
    }
    dlerror();
    void (*initPlugin)(VMAPI);
    *(void **)(&initPlugin) = dlsym(handle, "initPlugin");
    const char *err = dlerror();
    if (err != NULL) {
        printf(
            "FFI Error: Could not find 'initPlugin' inside '%s'\nReason: %s\n",
            path, err);
        dlclose(handle);
        return;
    }
    initPlugin(api);
    printf("Successfully loaded plugin: %s\n", path);
}

int main(void) {
    // TEST PROGRAM ADDS SHIT
    VMAPI api = {.registerNative = registerNative,
                 .evaluate = evaluate,
                 .createLiteral = createLiteral,
                 .createVariable = createVariable,
                 .createFunction = createFunction,
                 .pushRoot = pushRoot,
                 .popRoot = popRoot,
                 .createList = createList,
                 .copyTree = copyTree,
                 .getNodeType = getNodeType,
                 .getLeft = getLeft,
                 .getRight = getRight,
                 .getLiteral = getLiteral,
                 .getVarName = getVarName,
                 .getFuncName = getFuncName};
    // hardcoded for now
    loadPlugin("./out/CoreMath.so", api);

    printf("Starting GraphLang VM...\n");

    initAllocator();

    // condition: n < 2
    Node *cond =
        createFunction(createVariable("<"),
                       createArgs2(createVariable("n"), createLiteral(2)));

    // true branch: n
    Node *trueBranch = createVariable("n");

    // false branch: fib(n-1) + fib(n-2)
    Node *fib_n_minus_1 = createFunction(
        createVariable("fib"),
        createArgs1(createFunction(
            createVariable("-"),
            createArgs2(createVariable("n"), createLiteral(1)))));

    Node *fib_n_minus_2 = createFunction(
        createVariable("fib"),
        createArgs1(createFunction(
            createVariable("-"),
            createArgs2(createVariable("n"), createLiteral(2)))));

    Node *falseBranch = createFunction(
        createVariable("+"), createArgs2(fib_n_minus_1, fib_n_minus_2));

    Node *fibBody = createFunction(createVariable("?"),
                                   createArgs3(cond, trueBranch, falseBranch));

    Node *paramsList = createList(0, NULL);
    setLeft(paramsList, createVariable("n"));
    defineFunction("fib", paramsList, fibBody);

    // fib(25)
    Node *mainCall =
        createFunction(createVariable("fib"), createArgs1(createLiteral(25)));

    defineVariable("main", mainCall);

    printf("Building AST for fib(n)...\n");
    printf("Evaluating fib(25)... \n\n");
    enableGC();

    clock_t start = clock();
    Node *result = evaluate(mainCall, NULL);
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n=== RESULT: %d ===\n", getLiteral(result));
    printf("=== TIME: %f seconds ===\n\n", time_spent);

    printf("Running Final GC Pass...\n");
    markAll();
    sweep();
    printf("GC Complete! Dead nodes successfully recycled.\n");

    freeAllChunks();
    printf("VM Shutdown safely.\n");

    return 0;
}
