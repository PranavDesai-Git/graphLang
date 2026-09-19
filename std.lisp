(define map (f lst)
    (? (is_null lst)
       0
       (cons (f (car lst)) (map f (cdr lst)))))

(define filter (f lst)
    (? (is_null lst)
       0
       (? (f (car lst))
          (cons (car lst) (filter f (cdr lst)))
          (filter f (cdr lst)))))

(define fold (f acc lst)
    (? (is_null lst)
       acc
       (fold f (f acc (car lst)) (cdr lst))))

(define length (lst)
    (? (is_null lst)
       0
       (+ 1 (length (cdr lst)))))
