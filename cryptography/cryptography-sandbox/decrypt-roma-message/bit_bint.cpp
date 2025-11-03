#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>

class bit_bin_t {
public:
    bit_bin_t(std::vector<bool>&& ibits) : bits(ibits) {}

    bit_bin_t(std::string&& n) {
        for (size_t i = 0; i < n.size(); i++) {
            auto d = n[n.size() - 1 - i];
        }
    }

    bit_bin_t(size_t n) {
        while (n > 0) {
            bits.push_back(n % 2);
            n /= 2;
        }
    }

    auto to_string() const -> std::string {
        std::string str(bits.size(), 'x');
        for (size_t i = 0; i < bits.size(); i++) {
            str[bits.size() - 1 - i] = bits[i] ? '1' : '0';
        }
        return str;
    }

    auto to_int() const -> int {
        int out = 0;
        for (size_t i = 0; i < bits.size(); i++) {
            out += (1 << i) * bits[i];
        }
        return out;
    }

    friend auto operator+ (const bit_bin_t& left, const bit_bin_t& right) -> bit_bin_t {
        size_t max_length = std::max(left.bits.size(), right.bits.size());
        bit_bin_t output(std::vector<bool>(max_length + 1));
        bool cb = false;
        for (size_t i = 0; i < max_length; i++) {
            auto lb = i < left.bits.size() ? left.bits[i] : 0;
            auto rb = i < right.bits.size() ? right.bits[i] : 0;

            bool resb = lb ^ rb;
            bool carb = lb & rb;

            output.bits.at(i) = (resb ^ cb);
            cb = carb | (resb & cb);
        }
        if (cb) {
            output.bits.at(max_length) = (cb);
        }
        return output;
    }
private:
    std::vector<bool> bits;
};

int main()
{
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            auto l = bit_bin_t(i);
            auto r = bit_bin_t(j);
            auto bint = l + r;
        }
    }
    return 0;
}