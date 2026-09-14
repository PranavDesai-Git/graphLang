# GraphLang

An experimental, blazing fast purely functional Graph Reduction Virtual Machine written in C.

## Roadmap & Upcoming Features

- [ ] Implement Cheney's Copying GC (O(1) Allocation & Zero-pause Sweeps)
- [ ] Add Tail Call Optimization (TCO) with a Trampoline
- [ ] Implement Algebraic Rewriting (Tupling) for Automatic Tail Recursion Transformation
- [ ] Add Anonymous Functions (Lambdas / Closures)
- [x] Support Custom Datatypes (Plugin-defined ADTs & Userdata)
- [x] Build IO Monads for Side Effects
- [x] Create a Frontend Lexer, Parser, and REPL

GraphLang is a minimal, fast, integer-only programming language. At its core, it is an expression evaluator built entirely on a **strict binary tree**, with built-in support for variables, functions, and conditionals.

## Syntax Design

GraphLang uses a clean, minimalistic Lisp-style syntax (S-expressions). Because everything in the VM evaluates to a strict binary tree, S-expressions map exactly 1-to-1 to the underlying Graph Reduction architecture, making parsing trivial and evaluation lightning fast.

- **Function Definition:** `(define fib (n) (? (< n 2) n (+ (fib (- n 1)) (fib (- n 2)))))`
- **Variable Usage:** `x`
- **Function Call:** `(+ 5 10)`
- **Comments:** Any text following a semicolon `;` is ignored by the lexer.

## Operational Rules

- **Execution:** GraphLang evaluates source code by reading `.gl` files from top to bottom.
- **Truthiness:** There is no boolean type. `1` is True and `0` is False. Comparison operators naturally evaluate to `0` or `1`.
- **Variables:** Variables are completely mutable. Redefining a variable simply overwrites its previous entry in the hashtable.
- **Errors & Result Types:** Functions do not segfault on expected errors (like division by zero). Instead, they return the integer `0` tagged with a custom error flag. If a user tries to use an error-flagged node in a math operation without checking it first, the program safely aborts (Strict Error Propagation). Users handle these errors using a built-in `catch_flag` macro which clears the flag and executes a branch.

## Architecture & Evaluation

### The Microkernel Architecture
GraphLang is designed as an extensible **Microkernel Interpreter**. The core VM is completely agnostic to data types or operations. It simply provides:
1. A Memory Allocator and Garbage Collector.
2. A generic AST `Node` structure.
3. A `PluginAPI` that allows dynamic shared libraries (`.so` files) to register custom evaluation rules.

Built-in operations (like math) are loaded at runtime via the `CoreMath.so` plugin. This means users can completely redefine the semantics of the language by swapping out plugins.

### Strict Binary Tree
Everything in GraphLang is a strict binary tree. Function calls take exactly two arguments (mapped directly to the `left` and `right` AST node pointers), and arbitrary argument lists are constructed using standard Cons cells (`LIST` nodes).

### Lexical Scoping & Flat Closures
Instead of relying on a global hashtable or slow linked-list lookups, GraphLang uses **Lexical Addressing** and Flat Closures. 
When a function is called, the VM allocates an `ENV_FRAME` array directly on the Garbage Collector's heap. Variables are resolved down to a physical array index `O(1)` during semantic analysis, eliminating all string operations at runtime. 
Because these frames are fully traced AST nodes, GraphLang natively supports first-class functions, closures, and currying without fear of C-stack destruction!

### Strict Evaluation (Call-by-Value)
GraphLang uses strict **Call-by-Value** evaluation. Arguments passed to functions are fully evaluated in the caller's environment before being bound to the new scope. This prevents infinite loops (the Funarg problem) and ensures predictable execution speed.

### Memory Management (Mark-and-Sweep GC)
Because the AST creates many intermediate nodes during evaluation, GraphLang manages memory using a custom **Mark-and-Sweep Garbage Collector**. 
During evaluation, the VM pushes temporary C-stack variables to the GC's "Shadow Stack" (`pushRoot`) to protect them from being swept. The GC walks all active roots, marks reachable nodes, and reclaims dead nodes safely. (A Cheney Copying GC is planned for future optimization).

## Building & Running

A `Makefile` is provided for standard compilation.

```bash
# Build the project
make

# Build and execute
./run.sh
```
