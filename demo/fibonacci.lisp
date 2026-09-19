; Calculates the 15th Fibonacci number using recursion
(define fib (n)
    (? (< n 2)
       n
       (+ (fib (- n 1)) (fib (- n 2)))))

(io.print (fib 15))
