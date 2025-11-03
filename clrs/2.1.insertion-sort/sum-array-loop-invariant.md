sum = 0
for i = 0 to n
    sum = sum + A[i]
return sum

Init:
    When i = 1, the sum is a sum of empty array

Maintenance:
    Assume that sum = sum(A[1:(i-1)])
    Then after ith iteration the sum = sum(A[1:(i-1)]) + A[i]
    Which makes it sum(A[1:i])

Termination:
    i in incermenting monotonically until it reaches n

