#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#define MYSHA_IMPLEMENTATION
#include "sha.h"
#include <print>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

std::array<uint8_t, 32> cryptopp_sha256(std::vector<uint8_t> xs) {
    CryptoPP::SHA256 hash;
    std::array<uint8_t, CryptoPP::SHA256::DIGESTSIZE> digest{};
    
    // Compute SHA-256 hash
    hash.CalculateDigest(
        digest.data(),       // Output buffer
        xs.data(),        // Input data
        xs.size()         // Input length
    );
    
    return digest;
}

int main()
{
  std::ifstream file{"6.1.intro.mp4_download", std::ios::binary};
  // const auto file_size = file.tellg();
  // std::cout << file_size << std::endl;

  std::vector<std::vector<uint8_t>> blocks;
  std::vector<uint8_t> current_block;
  current_block.reserve(2048);
  char byte;
  while (file.get(byte)) {
    if (current_block.size() == 1024) {
      blocks.push_back(current_block);
      current_block.clear();
    }
    current_block.push_back(byte);
  }
  blocks.push_back(current_block);
  for (int64_t i = blocks.size() - 1; i >= 0; i--) {
    auto& block = blocks.at(i);
    auto hash = mysha::sha256(block);
    if (i > 0) {
      auto& prevb = blocks.at(i - 1);
      prevb.insert(prevb.end(), hash.begin(), hash.end());
    }
    else {
      mysha::output_hex(hash);
    }
  }

  return 0;
}

