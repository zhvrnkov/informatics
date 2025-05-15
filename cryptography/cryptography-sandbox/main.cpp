#include <cstdint>
#include <iostream>
#include <cassert>

std::string encrypt(std::string input, uint64_t salt)
{
    std::string padded = input;
    for (size_t i = 0; i < input.size() % 8; i++)
        padded.push_back(0);
    for (size_t i = 0; i < padded.size() / 8; i++) {
        uint64_t* data = (uint64_t*)(padded.data() + i * 8);
        *data = *data ^ salt;
    }
    return padded;
}

int main()
{
    std::string string = "Hello, world";
    std::cout << string << std::endl;
    string = encrypt(string, UINT64_MAX);
    std::cout << string << std::endl;
    string = encrypt(string, UINT64_MAX);
    std::cout << "|" << string << "|" << std::endl;
    main();
    return 0;
}