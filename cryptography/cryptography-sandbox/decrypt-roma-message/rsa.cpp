#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

namespace rsa {
size_t alphabet_size = 27;

char index2char(int index) {
  if (index == (alphabet_size - 1)) {
    return ' ';
  } else {
    return 'a' + index;
  }
}

int char2index(char c) {
  if (c == ' ') {
    return alphabet_size - 1;
  } else {
    return c - 'a';
  }
}

// вычисляет x в степени n мод m
size_t ebs(size_t x, size_t n, size_t m)
{
    size_t result = 1;
    while (n > 0) {
        if (n == 1) {
            result = (result * x) % m;
        } else if (n % 2 == 0) {
            x = (x * x) % m;
        } else {
            result = (result * x) % m;
            x = (x * x) % m;
            n -= 1;
        }
        n /= 2;
    }
    return result;
}

void print_message(std::vector<size_t> message)
{
    for (auto m : message) {
        while (m > 0) {
            m--;
            char c = index2char(m % alphabet_size);
            m = m / alphabet_size;
            std::cout << c;
        }
    }
    std::cout << std::endl;
}

std::vector<size_t> prepare_message(const std::string& message, size_t n)
{
    std::vector<size_t> msgs;
    size_t msg = 0;
    size_t chars_count_in_msg = 0;
    for (size_t i = 0; i < message.size(); ) {
        int c = char2index(message[i]) + 1;
        size_t delta = c * std::pow(alphabet_size, chars_count_in_msg++);
        if (msg + delta <= (n - 1)) {
            msg += delta;
            i++;
        } else {
            msgs.push_back(msg);
            msg = 0;
            chars_count_in_msg = 0;
        }
    }
    if (msg > 0) {
        msgs.push_back(msg);
    }
    return msgs;
}

// private key
std::vector<size_t> process(std::vector<size_t> message, int exponent, int modulo)
{
    std::vector<size_t> dec_messages = message;
    for (auto& message : dec_messages) {
        message = ebs(message, exponent, modulo);
    }
    return dec_messages;
}
}

// size_t my_gcd(size_t x, size_t y)
// {
//     size_t d = 0;
//     // d * k = x
//     // d * j = y
// }

void make_keys(uint64_t p, uint64_t q, uint64_t& n, uint64_t& e, uint64_t& d)
{
    n = p * q;

    // Euler's totient function. phi = number of positive integers that are smaller than n and comprime to
    // If phi(prime) = (prime - 1)
    // and phi(p * q) = (p - 1) * (q - 1)
    // then phi of any number requires factoring its to prime number and calculate mul(primes - 1)?
    // phi(8) = 4
    // the answer is no
    //
    // the Euler's theorem: a^(phi(n)) % n == 1
    // if a and n are coprime
    // m is ptext
    // c is ctext
    // c = m^e % n
    // m = c^d % n
    // c^d % n = m^ed % n
    size_t phi = (p - 1) * (q - 1);
    for (e = 2; e < phi; e++) {
        if (std::gcd(e, phi) == 1) {
            break;
        }
    }

    for (d = 2; d < phi; d++) {
        if ((d * e) % phi == 1) {
            break;
        }
    }
}

int main()
{
    // p и q должны быть большими простыми числами
    // выбираются произвольно, но чем больше тем надежнее шифр
    size_t p = 61;
    size_t q = 53;
    uint64_t n, e, d;
    make_keys(p, q, n, e, d);

    auto msg = rsa::prepare_message("hello vlad from rsa", n);
    rsa::print_message(msg);
    // encrypt with public key. e is public
    auto emsg = rsa::process(msg, e, n);
    rsa::print_message(emsg);
    // decrupt with private key. d is private
    auto dmsg = rsa::process(emsg, d, n);
    rsa::print_message(dmsg);

    {
        auto messages0 = std::vector<size_t>{228, 322};
        auto dmsg0 = rsa::process(messages0, d, n);
        auto emsg0 = rsa::process(dmsg0, e, n);
        std::cout << emsg0.size() << std::endl;
        std::cout << dmsg0.size() << std::endl;
        std::cout << messages0.size() << std::endl;
    }

    {
        size_t p = 19;
        size_t q = 37;
        // size_t n = 7919; 
        size_t n = p * q;
        // Euler's totient function считает количство coprime чисел между 1 и самим числом
        // 
        size_t phi = (p - 1) * (q - 1);
        size_t coprimes_count = 0;
        size_t coprime = 0;
        for (size_t i = n; i >= 1; i--) {
            if (std::gcd(i, n) == 1) {
              coprimes_count++;
              if (coprime == 0)
                coprime = i;
            }
        }
        printf("phi = %lu\n", phi);
        printf("ccount = %lu\n", coprimes_count);
        printf("coprime = %lu\n", coprime);
        printf("n = %lu\n", n);
        printf("coprime^phi%%n = %lu\n", rsa::ebs(coprime, phi, n));
    }

    return 0;
}


// g is small prime number
// n is big (2048 or 4096 bits)
// a is between 1 and n
// b is between 1 and n
// g^a % n
// ((x^a % n)^b % n) ?= x^ab % n