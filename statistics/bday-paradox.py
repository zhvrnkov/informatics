# xs = {x0, x1, x2, ..., x29} # total of 30
# P(no shared bday) = ???
# 
# xs = {x0, x1}
# P(no shared bday) = 364 / 365
# 
# xs = {x0, x1, x2}
# P(no shared bday) = 364/365 * 363/365???
# 
# xs = {x0,x1,x2,x3}
# P(no shared bday) = 364/365 * 363/365 * 362/365
#
# xs = {x0,x1,x2,x3,x4}
# P(no shared bday) = 364/365 * 363/365 * 362/365 * 361/365
#
# P(no shared bday) = prod[365 - n - 1 for x in n]/365^n

from math import *
from random import *

days = [day for day in range(1, 366)]

total_count = 100000
got_duplicates_count = 0
for i in range(total_count):
    A = []
    for i in range(5):
        A.append(days[randint(0, len(days) - 1)])

    got_duplicates_count += (len(A) > len(set(A)))

print(f"probability of duplicates = {got_duplicates_count / total_count}")

def prod(xs):
    y = 1.0
    for x in xs:
        y *= x
    return y

def P(n, N):
    return 1 - prod([(N - x) / N for x in range(n)]) 

for x in range(366):
    N = 365
    p = P(x, N)
    if p >= 0.99:
        print(f"{x} => {p}")
        # print(sqrt(N) * 1.2)
        # break

n = 32
print(sqrt(1 << n) * 1.2)
print(1 << (n // 2))
print(P(1 << (n // 2), 1<<n))
