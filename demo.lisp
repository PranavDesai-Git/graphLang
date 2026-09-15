;; 1. Higher-Order Functions and Closures
(define make-adder (x)
  (lambda (y) (+ x y)))

;; 2. Recursive Functions using 'define' and the native '?' (if)
(define factorial (n)
  (? (== n 0)
     1
     (* n (factorial (- n 1)))))

;; 3. Let Bindings and combining features
(let ((add10 (make-adder 10))
      (fact5 (factorial 5)))
  (+ (add10 5) fact5))


