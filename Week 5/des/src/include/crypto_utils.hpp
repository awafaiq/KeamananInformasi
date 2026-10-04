#pragma once
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <sys/types.h>
#include <vector>

namespace des_util
{

constexpr int BLOCK_SIZE = 8;

inline std::vector<uint8_t> pkcs7Pad(const std::vector<uint8_t> &data)
{
    int padLen = BLOCK_SIZE - static_cast<int>(data.size() % BLOCK_SIZE);
    if (padLen == 0)
        padLen = BLOCK_SIZE;
    std::vector<uint8_t> out = data;
    out.insert(out.end(), padLen, static_cast<uint8_t>(padLen));
    return out;
}

inline std::vector<uint8_t> pkcs7Unpad(const std::vector<uint8_t> &data)
{
    if (data.empty())
        throw std::runtime_error("Cannot unpad empty data");
    uint8_t padLen = data.back();
    if (padLen == 0 || padLen > BLOCK_SIZE || static_cast<size_t>(padLen) > data.size())
        throw std::runtime_error("Invalid PKCS7 padding");
    for (size_t i = data.size() - padLen; i < data.size(); ++i)
        if (data[i] != padLen)
            throw std::runtime_error("Invalid PKCS7 padding bytes");
    return std::vector<uint8_t>(data.begin(), data.end() - padLen);
}

inline std::vector<uint8_t> generateRandomIV()
{
    std::vector<uint8_t> iv(BLOCK_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto &b : iv)
        b = static_cast<uint8_t>(dist(gen));
    return iv;
}

inline std::vector<uint8_t> xorBytes(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b)
{
    std::vector<uint8_t> out(a.size());
    for (size_t i = 0; i < a.size(); i++)
        out[i] = a[i] ^ b[i];
    return out;
}

} // namespace des_util
