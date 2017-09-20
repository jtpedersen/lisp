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

(fib2 20)
(fib 20)
(fib2 46)
(fib2 47)
