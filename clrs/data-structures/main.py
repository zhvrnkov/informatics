from math import *
import random


# n = {0, 1}
# m = 1
# perms = 0, 1
#
# m = 2
# perms = 01, 10
#
# n = {0, 1, 2}
# m = 1
# perms = 0, 1, 2
#
# m = 2
# perms = 01, 02, 10, 12, 20, 21
# combs = 01, 02, 12
#
# m = 3
# perms = 012
#
# n = {0, 1, 2, 3}
# m = 2
# perms = 01, 02, 03, 10, 12, 13, 21, 20, 23, 30, 31, 32
# combs = 01, 02, 03, 12, 13, 23
#
# m = 3
# perms = 012, 013, 021, 023, 031, 032, 102, 103, 120, 123, 130, 132, 210, 213, 201, 203, 230, 231, 301, 302, 310, 312, 320, 321
# combs = 012, 123, 230, 301
def perms(n, m):
    out = 1
    for i in range(m):
        out *= n
        n -= 1
    return out

def combs(n, m):
    return perms(n, m) // perms(m, m)

n = 20
m = 10

def h(x):
    return hash(x) % m

n = 128
def hash_div(x):
    return x % n

def hash_mul(x):
    A = float.fromhex('0x0.CAFEBABEDEADBEEF')
    return floor(n * ((x * A) % 1))

def hash_muls(x, l, a = 2654435769):
    return ((x * a) % (1 << 32)) >> (32 - l)

# 11.3.6 - number theoretical hash on vector
#
# Z_p = {0, 1, 2, ..., p - 1}
# Q = Z_p
# p is prime
# U : set of d-tuples, tuple values are from Z_p
# h_b :: U -> Q (b in Z_p)
# h_b :: (d-tuple) -> Q
def h(p, b, tup) -> Q:
    out = 0
    for j in range(0, len(tup)):
        out += (tup[j] * (b**j)) % p
    return out % p










































