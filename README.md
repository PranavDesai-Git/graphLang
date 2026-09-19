# GraphLang

A minimal, fast, functional-leaning Lisp dialect and Virtual Machine written in C.

## GraphLang v1.0 Roadmap

- [x] **CoreList Plugin**: Implement `car`, `cdr`, and `is_null` to enable list manipulation.
- [x] **Standard Library (`std.lisp`)**: Implement functional utilities like `map`, `filter`, `fold`, and `length`.
- [x] **CoreString Plugin**: Implement a `FOREIGN` string type with operations like `str.make`, `str.concat`, and `str.len`.
- [x] **Fix Memory Leaks**: Clean up leaked `malloc` variable names in the parser and environment.

*(For future v2.0 architectural plans, including JIT and Actor Model concurrency, see [ideas.md](ideas.md))*

GraphLang is a minimal, dynamically-typed programming language. At its core, it is an expression evaluator built entirely on a **strict binary tree**, with built-in support for variables, first-class functions, closures, and `let` bindings.

## Syntax Design

GraphLang uses a clean, minimalistic Lisp-style syntax (S-expressions). S-expressions map exactly 1-to-1 to the underlying AST architecture, making parsing trivial and evaluation fast.

- **Function Definition:** `(define fib (n) (? (< n 2) n (+ (fib (- n 1)) (fib (- n 2)))))`
- **Anonymous Functions:** `(lambda (x y) (+ x y))`
- **Local Variables:** `(let ((x 10) (y 20)) (+ x y))`
- **Comments:** Any text following a semicolon `;` is ignored by the lexer.

## Operational Rules

- **Execution:** GraphLang evaluates source code by reading `.lisp` files or via the interactive REPL.
- **Truthiness:** There is no boolean type. `1` is True and `0` is False. Comparison operators naturally evaluate to `0` or `1`.
- **Variables:** Variables are mutable. Redefining a variable overwrites its previous entry in the global hashtable.
- **Errors:** If an error occurs (e.g., calling an undefined variable), the VM safely aborts the current evaluation using `longjmp` and returns control to the REPL without crashing the host process.

## Architecture & Evaluation

### Pluggable Core Architecture
GraphLang is designed with a **Minimal, Pluggable Core**. The core VM provides:
1. A Memory Allocator and Tracing Garbage Collector.
2. A generic AST `Node` structure.
3. A `PluginAPI` that allows dynamic shared libraries (`.so` files) to register custom evaluation rules and native functions.

Built-in operations (like math and I/O) are loaded at runtime via plugins like `CoreMath.so` and `CoreIO.so`. This means users can completely redefine the semantics of the language by swapping out plugins.

### Lexical Scoping & Closures
GraphLang uses **Lexical Addressing** and linked environment frames. 
When a function is called, the VM allocates an `ENV_FRAME` array on the Garbage Collector's heap. Local variables inside functions and `let` blocks are resolved down to a physical array index `O(1)` during parsing, eliminating slow string lookups at runtime. (Global variables continue to utilize a string-based hashtable). Because these frames are fully traced AST nodes, GraphLang natively supports first-class functions and closures (by capturing the environment frame) without corrupting the C-stack.

### Strict Evaluation (Call-by-Value)
GraphLang uses strict **Call-by-Value** evaluation. Arguments passed to functions are fully evaluated in the caller's environment before being bound to the new scope, ensuring predictable execution speed.

### Memory Management (Mark-and-Sweep GC)
GraphLang manages memory using a custom **Mark-and-Sweep Garbage Collector**. 
During evaluation, the VM pushes temporary C-stack variables to the GC's "Shadow Stack" (`pushRoot`) to protect them from being prematurely swept. The GC walks all active roots, marks reachable nodes, and reclaims dead nodes safely.

## Building & Running

A `Makefile` is provided for standard compilation.

```bash
# Build the project (compiles the VM and plugins)
make

# Run the interactive REPL
./out/graphLang

# Run a specific script
./out/graphLang demo.lisp
```
