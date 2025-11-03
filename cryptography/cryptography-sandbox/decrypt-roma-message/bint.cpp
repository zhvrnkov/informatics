#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include <vector>

size_t powi(size_t x, size_t n)
{
    size_t result = 1;
    while (n > 0) {
        if (n == 1) {
            result = (result * x);
        } else if (n % 2 == 0) {
            x = (x * x);
        } else {
            result = (result * x);
            x = (x * x);
            n -= 1;
        }
        n /= 2;
    }
    return result;
}

class bint_t {
public:
    bint_t(std::string&& n) : data(n) {
    }

    bint_t(size_t n) {
        while (n > 0) {
            data.insert(data.begin(), '0' + n % 10);
            n /= 10;
        }
    }

    auto to_string() const -> std::string {
        return data;
    }

    auto to_int() const -> size_t {
        size_t out = 0;
        for (size_t i = 0; i < data.size(); i++) {
            out += powi(10, i) * digit_at(data.size() - 1 - i);
        }
        return out;
    }

    friend auto operator+= (bint_t& left, const bint_t& right) {
        left = left + right;
    }

    friend auto operator+ (const bint_t& left, const bint_t& right) -> bint_t {
        size_t max_length = std::max(left.data.size(), right.data.size());
        std::string result(max_length, '0');
        size_t carry = 0;
        for (size_t i = 0; i < max_length; i++) {
            auto ld = left.digit_at_from_end(i);
            auto rd = right.digit_at_from_end(i);

            auto sum = ld + rd + carry;
            result[result.size() - 1 - i] = (char)('0' + sum % 10);
            carry = sum / 10;
        }
        while (carry > 0) {
            result.insert(result.begin(), (char)('0' + carry % 10));
            carry /= 10;
        }
        return bint_t(std::move(result));
    }

    friend auto operator* (const bint_t& top, const bint_t& bottom) -> bint_t {
        size_t max_length = std::max(top.data.size(), bottom.data.size());
        auto result = bint_t(std::string(max_length, '0'));

        for (size_t ib = 0; ib < bottom.data.size(); ib++) {
            auto b = bottom.digit_at_from_end(ib);
            for (size_t it = 0; it < top.data.size(); it++) {
                auto t = top.digit_at_from_end(it);
                result.add(b * t, it + ib);
            }
        }

        return result;
    }

    friend auto operator% (const bint_t& top, const bint_t& bottom) -> bint_t {
        
    }

    auto sqrt () -> bint_t {
        
    }

private:
    void add(size_t num, size_t exp) {
        size_t carry = 0;
        while (num > 0 || carry > 0) {
            auto n = num % 10;
            auto d = digit_at_from_end(exp);
            auto sum = n + d + carry;
            int index = data.size() - 1;
            if (exp > index) {
                data.insert(0, std::string(exp - index, '0'));
                index = data.size() - 1;
            }
            index -= exp;
            data.at(index) = '0' + (sum % 10);
            carry = sum / 10;
            num /= 10;
            exp++;
        }
    }

    auto digit_at(size_t index) const -> size_t {
        return data.at(index) - '0';
    }

    auto digit_at_from_end(size_t i) const -> size_t {
        auto index = data.size() - 1 - i;
        if (index >= 0 && index < data.size()) {
            return data[index] - '0';
        } else {
            return 0;
        }
    }

    auto set_digit_at(size_t index, size_t digit) {
        data.at(index) = '0' + digit;
    }

    std::string data;
};

int main()
{
    std::vector<std::thread> threads;
    size_t S = (1 << 15);
    size_t is = S / 16;
    for (size_t it = 0; it < 16; it++) {
        threads.emplace_back(std::thread([it, is, S]() {
          for (int i = it * is; i < ((it + 1) * is); i++) {
            for (int j = 0; j < S; j++) {
              auto l = bint_t(i);
              auto r = bint_t(j);
              auto bint = l + r;
              assert((l + r).to_int() == i + j);
              assert((l * r).to_int() == i * j);
            }
          }
        }));
    }
    for (size_t i = 0; i < 16; i++) {
        threads[i].join();
    }
    return 0;
}