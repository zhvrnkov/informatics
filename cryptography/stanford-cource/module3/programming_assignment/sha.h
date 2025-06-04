#pragma once
#include <array>
#include <vector>
#include <cstdint>

namespace mysha {
  std::array<uint8_t, 32> sha256(std::vector<uint8_t> input);
  void output_hex(auto t);
  void endian_swap(uint32_t& x);
}

#define MYSHA_IMPLEMENTATION
#ifdef MYSHA_IMPLEMENTATION
#include <algorithm>
#include <cstring>
#include <iterator>
#include <print>

namespace mysha {
  void output_hex(auto t)
  {
    for (auto i = 0; i < t.size(); i++) {
      std::print("{:02x}", t[i]);
    }
    std::println();
  }

  void output_bin(auto t)
  {
    for (auto i = 0; i < t.size(); i++) {
      //   if (i % 4 == 0) {
      //     std::print("| ");
      //   }
      std::print("{:02b} ", t.at(i));
    }
    std::println();
  }

  constexpr const auto BLOCKSIZE = 64; // 512 / 8

  uint32_t right_rot(uint32_t n, uint32_t d) 
  { 
    return (n >> d) | (n << (32 - d)); 
  }

  void endian_swap(uint32_t& x)
  {
    // x = 3 2 1 0
    x = (x >> (8 * 3) & 0xFF) << (8 * 0) 
      | (x >> (8 * 2) & 0xFF) << (8 * 1) 
      | (x >> (8 * 1) & 0xFF) << (8 * 2) 
      | (x >> (8 * 0) & 0xFF) << (8 * 3);
  }

  void sha_pad(std::vector<uint8_t>& input)
  {
    uint64_t msize = input.size();
    input.push_back(0x80);
    auto pad_size = BLOCKSIZE - (input.size() + 8) % BLOCKSIZE;
    input.insert(input.end(), pad_size, 0x00);
    auto msize_bits = msize * 8;
    auto msize_bytes = std::span<uint8_t, 8>((uint8_t*)&msize_bits, 8);
    for (auto it = msize_bytes.rbegin(); it != msize_bytes.rend(); it++) {
      input.push_back(*it);
    }
  }

  std::array<uint8_t, 32> sha256(std::vector<uint8_t> input) {
    sha_pad(input);

    std::array<uint32_t, 8> H{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    const std::array<uint32_t, 64> K = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
      0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
      0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
      0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
      0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
      0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    auto blocks_count = input.size() / BLOCKSIZE;

    for (auto ib = 0; ib < blocks_count; ib++) {
      std::span<uint8_t, BLOCKSIZE> block{input.begin() + ib * BLOCKSIZE, BLOCKSIZE};
      std::array<uint32_t, 64> ws{0};
      std::memcpy(ws.begin(), block.data(), block.size());
      for (auto& w : ws) {
        endian_swap(w);
      }
      for (auto i = block.size() / sizeof(ws[0]); i < ws.size(); i++) {
        uint32_t s0 = right_rot(ws[i - 15], 7) ^ right_rot(ws[i - 15], 18) ^ (ws[i - 15] >> 3);
        uint32_t s1 = right_rot(ws[i - 2], 17) ^ right_rot(ws[i - 2], 19) ^ (ws[i - 2] >> 10);
        ws[i] = ws[i - 16] + s0 + ws[i - 7] + s1;
      }

      auto a = H[0];
      auto b = H[1];
      auto c = H[2];
      auto d = H[3];
      auto e = H[4];
      auto f = H[5];
      auto g = H[6];
      auto h = H[7];

      for (auto i = 0; i < ws.size(); i++) {
        auto S1 = right_rot(e, 6) ^ right_rot(e, 11) ^ right_rot(e, 25);
        auto ch = (e & f) ^ ((~ e) & g);
        auto temp1 = h + S1 + ch + K[i] + ws[i];
        auto S0 = right_rot(a, 2) ^ right_rot(a, 13) ^ right_rot(a, 22);
        auto maj = (a & b) ^ (a & c) ^ (b & c);
        auto temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
      }
      H[0] += a;
      H[1] += b;
      H[2] += c;
      H[3] += d;
      H[4] += e;
      H[5] += f;
      H[6] += g;
      H[7] += h;
    }
    for (auto& h : H) {
      endian_swap(h);
    }
    std::array<uint8_t, 32> out;
    std::memcpy(out.data(), (void*)H.data(), out.size());
    return out;
  }
} // namespace mysha

#endif

#ifdef MYSHA_CLI
int main(int argc, char** argv)
{
  if (argc <= 1) {
    return 1;
  }
  std::string_view input{argv[1]};
  std::vector<uint8_t> input_v{input.begin(), input.end()};
  mysha::output_hex(mysha::sha256(input_v));

  return 0;
}
#endif
