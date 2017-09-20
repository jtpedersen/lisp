(define (and a b)
  (if a
      b
    false))

(define (or a b)
  (if a
      true
    b))

(and true false)
(and true true)
(and false true)
(and false false)

(or true false)
(or true true)
(or false true)
(or false false)


(define (pascal row col)
  (if (or (< row 0)  (< col 0))
      0
    (if (or (= row 0)  (= col 0))
        1
      (+ (pascal (- row 1) col) (pascal (- row 1) (+ 1 col))))))

(pascal 0 0)
(pascal 1 1)
(pascal 2 2)


(define (rec-sum x)
  (if x
      (+ (rec-sum (- x 1)) x)
    0))

(define (sum-n x)
  (/ (* x (+ x 1)) 2))

(rec-sum 3)
(sum-n 3)
(sum-n 10)
(rec-sum 10)

(define (fib x)
  (if (< x 2)
      x
    (+ (fib (- x 1)) (fib (- x 2)))))

(define (fib-iter a b count)
  (if (= count 0)
      b
    (fib-iter (+ a b) a (- count 1))))

(define (fib2 n)
  (fib-iter 1 0 n))

(fib2 1)
(fib 1)
(fib2 2)
(fib 2)
(fib2 13)
(fib 13)
