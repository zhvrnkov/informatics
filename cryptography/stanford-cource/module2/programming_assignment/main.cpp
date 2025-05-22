#include "cryptopp/misc.h"
#include <array>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <cryptopp/aes.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/secblock.h>
#include <string_view>

using namespace CryptoPP;

template<typename Decryption>
void solve_easy(std::string ciphertext, std::string key)
{
    constexpr auto IVL = 16;
    std::string cipherbytes;
    StringSource ssc(ciphertext, true, new HexDecoder(new StringSink(cipherbytes)));

    std::string keybytes;
    StringSource ssk(key, true, new HexDecoder(new StringSink(keybytes)));

    Decryption d;
    d.SetKeyWithIV((const byte*)keybytes.data(), keybytes.size(), (const byte*)cipherbytes.data(), IVL);

    std::string recovered(cipherbytes.size() - IVL, '_');
    d.ProcessString((byte*)recovered.data(), (byte*)cipherbytes.data() + IVL, recovered.size());

    std::cout << recovered << std::endl;
}

void solve_cbchard(std::string inputtext, std::string key)
{
    constexpr auto IVL = AES::BLOCKSIZE;
    std::string input;
    StringSource ssc(inputtext, true, new HexDecoder(new StringSink(input)));

    std::string keybytes;
    StringSource ssk(key, true, new HexDecoder(new StringSink(keybytes)));

    std::string iv(input.data(), IVL);

    std::string cipher(input.data() + IVL, input.size() - IVL);

    std::string recovered(cipher.size(), '_');

    ECB_Mode<AES>::Decryption d;
    d.SetKey((const byte*)keybytes.data(), keybytes.size());

    auto blocks_count = cipher.size() / AES::BLOCKSIZE;

    for (auto ib = 0; ib < blocks_count; ib++) {
        d.ProcessData((byte*)(recovered.data() + ib * AES::BLOCKSIZE), (byte*)(cipher.data() + ib * AES::BLOCKSIZE), AES::BLOCKSIZE);
        for (auto ibyte = 0; ibyte < AES::BLOCKSIZE; ibyte++) {
            recovered[ib * AES::BLOCKSIZE + ibyte] ^= iv[ibyte];
        }
        iv = std::string(cipher.data() + ib * AES::BLOCKSIZE, IVL);
    }

    // d.ProcessData(byte *outString, const byte *inString, size_t length)

    auto pad_length = static_cast<int>(recovered[recovered.size() - 1]);

    std::print(std::cout, "{}\n", std::string_view(recovered.data(), recovered.size() - pad_length));
}

void solve_ctrhard(std::string inputtext, std::string key)
{
    constexpr auto IVL = AES::BLOCKSIZE;
    std::string input;
    StringSource ssc(inputtext, true, new HexDecoder(new StringSink(input)));

    std::string keybytes;
    StringSource ssk(key, true, new HexDecoder(new StringSink(keybytes)));

    std::string iv(input.data(), IVL);

    std::string cipher(input.data() + IVL, input.size() - IVL);

    std::string recovered(cipher.size(), '_');

    ECB_Mode<AES>::Encryption d;
    d.SetKey((const byte *)keybytes.data(), keybytes.size());

    std::string k(AES::BLOCKSIZE, 0);
    for (auto i = 0; i < cipher.size(); i++) {
      if (i % AES::BLOCKSIZE == 0) {
        d.ProcessData((byte *)k.data(), (byte *)iv.data(), iv.size());
        IncrementCounterByOne((byte *)iv.data(), iv.size());
      }
      recovered[i] = k[i % k.size()] ^ cipher[i];
    }

    auto pad_length = static_cast<int>(recovered[recovered.size() - 1]);
    pad_length = 0;

    std::print(
        std::cout, "{}\n",
        std::string_view(recovered.data(), recovered.size() - pad_length));
}

int main() { 
    AutoSeededRandomPool prng;

    std::array<byte, AES::DEFAULT_KEYLENGTH> key;
    prng.GenerateBlock(key.data(), key.size());

    std::string plain = "Hello world from Uganda";
    auto pad_size = (AES::BLOCKSIZE - (plain.size() % AES::BLOCKSIZE));
    if (pad_size == 0) pad_size = AES::BLOCKSIZE;
    plain += std::string(pad_size, (char)pad_size);
    std::string cipher(plain.size(), '_');

    try {
        ECB_Mode<AES>::Encryption e;
        e.SetKey(key.data(), key.size());

        e.ProcessString((byte*)cipher.data(), (byte*)plain.data(), plain.size());
    }
    catch (const Exception e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    std::string recovered(plain.size(), '_');
    try {
        ECB_Mode<AES>::Decryption d;
        d.SetKey(key.data(), key.size());

        d.ProcessString((byte*)recovered.data(), (byte*)cipher.data(), cipher.size());
    }
    catch (const Exception e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << recovered << std::endl;

    solve_cbchard("4ca00ff4c898d61e1edbf1800618fb2828a226d160dad07883d04e008a7897ee2e4b7465d5290d0c0e6c6822236e1daafb94ffe0c5da05d9476be028ad7c1d81", "140b41b22a29beb4061bda66b6747e14");
    solve_cbchard("5b68629feb8606f9a6667670b75b38a5b4832d0f26e1ab7da33249de7d4afc48e713ac646ace36e872ad5fb8a512428a6e21364b0c374df45503473c5242a253", "140b41b22a29beb4061bda66b6747e14");
    solve_ctrhard("69dda8455c7dd4254bf353b773304eec0ec7702330098ce7f7520d1cbbb20fc388d1b0adb5054dbd7370849dbf0b88d393f252e764f1f5f7ad97ef79d59ce29f5f51eeca32eabedd9afa9329", "36f18357be4dbd77f050515c73fcf9f2");
    solve_ctrhard("770b80259ec33beb2561358a9f2dc617e46218c0a53cbeca695ae45faa8952aa0e311bde9d4e01726d3184c34451", "36f18357be4dbd77f050515c73fcf9f2");
    return 0;
}
