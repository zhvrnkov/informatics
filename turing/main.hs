phi m n = m * n

eta :: Integer -> Integer
eta 0 = 1
eta x = phi x $ eta $ x - 1

sieve :: Integer -> Integer -> Bool
sieve 0 _ = False
sieve 1 _ = False
sieve x 1 = True
sieve x n = ((mod x n) /= 0) && (sieve x $ n - 1)

isPrime x = sieve x $ x - 1
