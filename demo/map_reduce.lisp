; A functional pipeline demo: squares a list of numbers and sums them up.
(define map (f lst)
    (? (is_null lst)
       0
       (cons (f (car lst)) (map f (cdr lst)))))

(define fold (f acc lst)
    (? (is_null lst)
       acc
       (fold f (f acc (car lst)) (cdr lst))))

(define square (x) (* x x))
(define add (a b) (+ a b))

; List: [1, 2, 3, 4]
(define my_list ()
    (cons 1 (cons 2 (cons 3 (cons 4 0)))))

(define main ()
    (io.print (fold add 0 (map square (my_list)))))

(main)
