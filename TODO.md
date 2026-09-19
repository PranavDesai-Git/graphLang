### 15-09-26
- [x] Implement Lambdas
- [x] Local Variables using Let (might reuse the same for lambdas)
- [x] Prevent typeID collisions. 

### GraphLang v1.0 Roadmap
- [x] **CoreList Plugin**: Implement `car`, `cdr`, and `is_null` to enable list manipulation.
- [x] **Standard Library (`std.lisp`)**: Implement functional utilities like `map`, `filter`, `fold`, and `length`.
- [ ] **CoreString Plugin**: Implement a `FOREIGN` string type with operations like `str.make`, `str.concat`, and `str.len`.
- [ ] **Fix Memory Leaks**: Clean up leaked `malloc` variable names in `Parser.c` and `Environment.c`.
