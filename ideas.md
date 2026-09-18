# GraphLang Architectural Proposals

## Opaque Value Architecture
Consolidate `LITERAL` and `FOREIGN` AST node types into a single `VALUE` type. The `VALUE` node contains an opaque `void*` payload and a function pointer to a `MessageHandler`. The evaluation engine performs no type checking or type-specific branching, delegating all operations on a `VALUE` by invoking its `MessageHandler` pointer with the operation token and evaluated arguments.

## Message Passing Dispatch
Replace global native function bindings in the environment with a dynamic dispatch system localized to `VALUE` nodes. A `VALUE` node's `MessageHandler` evaluates a string token (e.g., `+`, `print`) and executes the corresponding C function bound to that object instance. 

## Prototypal Inheritance
Introduce a `Node* prototype` pointer to the `VALUE` struct. Implement a recursive dispatch resolution loop in the evaluation engine: if a `VALUE` node's `MessageHandler` returns an unhandled signal, the evaluator re-invokes the message token on the node referenced by the `prototype` pointer.

## Variadic Argument Unpacking API
Expose a C variadic function `vm.unpackArgs(Node* args, const char* format, ...)` via `PluginAPI.h`. The function traverses the `CONS` linked list of AST arguments, performs type enforcement based on the format string (e.g., `"iis"`), and casts the underlying `void*` payloads into the provided C pointers.

## Error Propagation via setjmp
Expose a `vm.throwError(const char* msg)` interface via `PluginAPI.h`. The function executes a `longjmp` on the VM's global `error_jmp` buffer to unwind the C call stack from native plugin space back to the main evaluation loop.

## Automatic GC Local Scopes
Modify the foreign function interface boundary within `Evaluator.c` to cache the current `rootCount` index of the garbage collector's shadow stack prior to invoking a native C plugin function. Restore `rootCount` to the cached index immediately upon function return.

## JIT (Just-In-Time) Compilation Engine
Transition from an AST-walking evaluation model to a Baseline JIT Compiler. The engine will traverse the AST and emit raw machine code (e.g., x86_64 or ARM64 instruction bytes) directly into executable memory buffers (`mmap` with `PROT_EXEC`). Core language constructs will be compiled into machine instructions, while complex plugin message dispatches will be compiled as native C function calls (`FFI` calls) embedded within the generated machine code.

## Actor Model Concurrency
Implement concurrency by treating isolated `VALUE` objects as independent Actors, sharing absolutely zero memory. Communication between concurrent processes will occur exclusively via asynchronous, thread-safe message queues (mailboxes). This unifies the Opaque Value architecture with the threading model, allowing seamless scaling without the need for shared-state mutexes or locks.

## Result Type Error Handling
Deprecate the `setjmp`/`longjmp` exception unwinding architecture in favor of explicit Result Types. Native plugins and standard library functions will return algebraic data structures (e.g., a variant `VALUE` containing either `[OK, Data]` or `[ERROR, Message]`). This eliminates hidden control flow and forces explicit error checking at the call site, ensuring that the VM evaluation loop remains linear and predictable.

## Concurrent & Actor-Local Garbage Collection
Retain the Tracing Garbage Collector (Mark & Sweep) but transition away from a global "stop-the-world" model. By leveraging the Actor Model, each Actor process will maintain its own isolated memory heap and shadow stack. Garbage collection can execute concurrently across different Actors, or be relegated to a background concurrent marking thread. This guarantees high throughput and eliminates global GC pauses, mirroring the Erlang/BEAM VM memory model.
