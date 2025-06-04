#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <print>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <span>
#include <sys/types.h>
#include <vector>

using namespace CryptoPP;

using aes_block_t = std::span<uint8_t, AES::BLOCKSIZE>;
using tag_t = std::array<uint8_t, AES::BLOCKSIZE>;

tag_t compression_f(tag_t H, aes_block_t m) 
{
    tag_t out; 
    try {
        ECB_Mode<AES>::Encryption e;
        e.SetKey(m.data(), m.size());
        e.ProcessData((byte*)out.data(), (byte*)H.data(), m.size());
        for (auto i = 0; i < out.size(); i++) {
          out[i] ^= H[i];
        }
        return out;
    } catch (const Exception e) {
        std::println(std::cerr, "[ERROR] my_compression failed {}", e.what());
        throw e;
    }
}

constexpr const tag_t IV{0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85, 0x3c, 0x6e, 0xf3, 0x72, 0xa5, 0x4f, 0xf5, 0x3a};

tag_t hash_f(std::vector<uint8_t> ms, tag_t iv = IV)
{
    uint64_t msize = ms.size();
    ms.push_back(0x80);
    auto pad_size = AES::BLOCKSIZE - (ms.size() + 8) % AES::BLOCKSIZE;
    for (auto i = 0; i < pad_size; i++) {
        ms.push_back(0x00);
    }
    ms.append_range(std::span<uint8_t, 8>((uint8_t*)&msize, 8));
    tag_t H = iv;
    auto blocks_count = ms.size() / AES::BLOCKSIZE;
    for (auto ib = 0; ib < blocks_count; ib++) {
        H = compression_f(H, aes_block_t{ms.data() + ib * AES::BLOCKSIZE, AES::BLOCKSIZE});
    }
    return H;
}

void output_tag(auto t)
{
    for (auto h : t) {
      std::print("{:02x}", static_cast<int>(h));
    }
    std::println();
}

namespace my_hmac {
constexpr const uint8_t ipad = 0x36;
constexpr const uint8_t opad = 0x5c;

tag_t s(aes_block_t key, std::vector<uint8_t> ms) {
  aes_block_t ikey = key;
  std::vector<uint8_t> okey{key.begin(), key.end()};
  for (auto i = 0; i < key.size(); i++) {
    ikey[i] ^= ipad;
    okey[i] ^= opad;
  }
  std::vector<uint8_t> input{ikey.begin(), ikey.end()};
  input.append_range(ms);
  auto ihash = hash_f(input, IV);
  auto ihash_v = std::vector<uint8_t>{ihash.begin(), ihash.end()};
  auto ohash = hash_f(okey, IV);
  return hash_f(ihash_v, ohash);
}

bool v(aes_block_t key, std::vector<uint8_t> ms, tag_t tag) {
  auto new_s = s(key, ms);
  auto result = new_s == tag;
  return result;
}
} // namespace my


int main(int argc, char** argv)
{
  if (argc <= 1) return -1;
  char command = *argv[1];
  switch (command) {
  case 'h': {
    std::string input{argv[2]};
    std::vector<uint8_t> ms;
    for (auto c : input) {
      ms.push_back(c);
    }
    auto hash = hash_f(ms);
    output_tag(hash);
    break;
  };
  case 's': {
    std::string key{argv[2]};
    std::string input{argv[3]};
    std::vector<uint8_t> k;
    std::vector<uint8_t> ms;
    for (auto c : key) {
      k.push_back(c);
    }
    for (auto c : input) {
      ms.push_back(c);
    }
    auto sign = my_hmac::s(aes_block_t{k.begin(), k.end()}, ms);
    output_tag(sign);
    break;
  };
  case 'v': {
    std::string key{argv[2]};
    std::string input{argv[3]};
    std::string tag;
    StringSource ssc(argv[4], true, new HexDecoder(new StringSink(tag)));
    std::vector<uint8_t> k;
    std::vector<uint8_t> ms;
    tag_t t;
    for (auto c : key) {
      k.push_back(c);
    }
    for (auto c : input) {
      ms.push_back(c);
    }
    for (auto i = 0; i < tag.length(); i++) {
      t.at(i) = tag.at(i);
    }
    if (my_hmac::v(aes_block_t{k.begin(), k.end()}, ms, t)) {
      std::println("true");
    } else {
      std::println("false");
    }
    break;
  };
  }
}
