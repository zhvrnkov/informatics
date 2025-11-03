#include <cmath>
#include <iostream>
#include <cassert>
#include <numeric>

size_t ipow(size_t x, size_t n, size_t m) {
    size_t result = 1;
    while (n > 0) {
        auto bit = n % 2;
        if (bit == 1) {
            result = (result * x) % m;
        }
        x = (x * x) % m;
        n /= 2;
    }
    return result % m;
}

size_t ipow(size_t x, size_t n) {
    size_t result = 1;
    while (n > 0) {
        auto bit = n % 2;
        if (bit == 1) {
            result = (result * x);
        }
        x = (x * x);
        n /= 2;
    }
    return result;
}

int order(size_t x, size_t p) {
    // check for coprime
    // if (std::gcd(x, p) != 1) return -1;
    // or because Euler's theorem states that two coprime numbers have the following equivelence:
    if (ipow(x, (p - 1), p) != 1) return -1;

    for (size_t i = 1; i < p; i++) {
        if (ipow(x, i, p) == 1) return i;
    }

    return -1;
}

// if g is primiteve_root
// then g^i % p (where 1 < i < p) will go through all numbers in 1..<p
// which is very important for crypto, cause in either case there will be smaller subset
// => less veriants => easier to brutforce
//
// and I think it's also important for random number generators
// if its implementation is related ofc
bool is_primiteve_root(size_t x, size_t p) {
    return order(x, p) == p - 1;
}

int main() {
    size_t p = 13;
    for (size_t i = 1; i < p; i++) {
        if (is_primiteve_root(i, p)) std::cout << "prim_root = " << i << std::endl;
    }
    return 1;
    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 10; j++) {
            for (size_t k = 0; k < 10; k++) {
              if (ipow(i, j, k) != ((size_t)round(std::pow(i, j))) % k) {
                std::cout << i << "^" << j << " % " << k << std::endl;
              }
            }
        }
    }
    return 1;
}