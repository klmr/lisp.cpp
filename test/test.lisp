(define square (lambda (n) (* n n)))
square
(square 4)
42
"Hello world!"
(square (if (quote ()) 1 2))
(define x (square 3))
x
(set! x (* x 2))
x
(set! x 1)
x
(begin (set! x (+ x 1)) (set! x (+ x 1)) x)
(not (!= x 3))
(length (quote (1 2 3)))
(list 1 2 3 4)
"All tests passed!"
