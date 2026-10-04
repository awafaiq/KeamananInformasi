#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace des
{

// standard des tables
static const int IP_TABLE[64] = {58, 50, 42, 34, 26, 18, 10, 2,  60, 52, 44, 36, 28, 20, 12, 4,  62, 54, 46, 38, 30, 22,
                                 14, 6,  64, 56, 48, 40, 32, 24, 16, 8,  57, 49, 41, 33, 25, 17, 9,  1,  59, 51, 43, 35,
                                 27, 19, 11, 3,  61, 53, 45, 37, 29, 21, 13, 5,  63, 55, 47, 39, 31, 23, 15, 7};

static const int FP_TABLE[64] = {40, 8,  48, 16, 56, 24, 64, 32, 39, 7,  47, 15, 55, 23, 63, 31, 38, 6,  46, 14, 54, 22,
                                 62, 30, 37, 5,  45, 13, 53, 21, 61, 29, 36, 4,  44, 12, 52, 20, 60, 28, 35, 3,  43, 11,
                                 51, 19, 59, 27, 34, 2,  42, 10, 50, 18, 58, 26, 33, 1,  41, 9,  49, 17, 57, 25};

static const int E_TABLE[48] = {32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11,
                                12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
                                22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

static const int P_TABLE[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
                                2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25};

static const int PC1_TABLE[56] = {57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43,
                                  35, 27, 19, 11, 3,  60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,  62, 54,
                                  46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};

static const int PC2_TABLE[48] = {14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,
                                  26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
                                  51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

static const int SHIFT_SCHEDULE[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

static const int S_BOX[8][4][16] = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                                     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                                     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                                     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
                                    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                                     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                                     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                                     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
                                    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                                     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                                     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                                     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
                                    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                                     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                                     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                                     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
                                    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                                     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                                     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                                     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
                                    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                                     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                                     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                                     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
                                    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                                     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                                     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                                     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
                                    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                                     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                                     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                                     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

// bit utilities
using Bits = std::vector<int>;

inline Bits bytesToBits(const std::vector<uint8_t> &bytes)
{
    Bits bits;
    bits.reserve(bytes.size() * 8);
    for (uint8_t b : bytes)
        for (int i = 7; i >= 0; --i)
            bits.push_back((b >> i) & 1);
    return bits;
}

inline std::vector<uint8_t> bitsToBytes(const Bits &bits)
{
    std::vector<uint8_t> bytes(bits.size() / 8, 0);
    for (size_t i = 0; i < bits.size(); ++i)
    {
        int bitInByte = 7 - static_cast<int>(i % 8);
        if (bits[i])
            bytes[i / 8] |= (1 << bitInByte);
    }
    return bytes;
}

inline std::string bitsToHex(const Bits &bits)
{
    std::ostringstream oss;
    for (size_t i = 0; i < bits.size(); i += 4)
    {
        int val = 0;
        for (int j = 0; j < 4; j++)
            val = (val << 1) | bits[i + j];
        oss << std::hex << val;
    }
    return oss.str();
}

inline Bits permute(const Bits &input, const int *table, int outSize)
{
    Bits out(outSize);
    for (int i = 0; i < outSize; i++)
        out[i] = input[table[i] - 1];
    return out;
}

inline Bits leftRotate(const Bits &half, int n)
{
    Bits out(half.size());
    int size = static_cast<int>(half.size());
    for (int i = 0; i < size; i++)
        out[i] = half[(i + n) % size];
    return out;
}

inline Bits xorBits(const Bits &a, const Bits &b)
{
    Bits out(a.size());
    for (size_t i = 0; i < a.size(); i++)
        out[i] = a[i] ^ b[i];
    return out;
}

// key schedule
struct KeySchedule
{
    std::array<Bits, 16> roundKeys;
};

inline KeySchedule generateRoundKeys(const Bits &key64, bool verbose)
{
    KeySchedule ks;
    Bits key56 = permute(key64, PC1_TABLE, 56);
    Bits C(key56.begin(), key56.begin() + 28);
    Bits D(key56.begin() + 28, key56.end());

    if (verbose)
        std::cout << "  [KeySchedule] PC-1 -> 56 bits. C0=" << bitsToHex(C) << " D0=" << bitsToHex(D) << "\n";

    for (int round = 0; round < 16; round++)
    {
        C = leftRotate(C, SHIFT_SCHEDULE[round]);
        D = leftRotate(D, SHIFT_SCHEDULE[round]);
        Bits CD;
        CD.reserve(56);
        CD.insert(CD.end(), C.begin(), C.end());
        CD.insert(CD.end(), D.begin(), D.end());
        Bits roundKey = permute(CD, PC2_TABLE, 48);
        ks.roundKeys[round] = roundKey;
        if (verbose)
            std::cout << "  [KeySchedule] Round " << (round + 1) << " (shift " << SHIFT_SCHEDULE[round]
                      << "): C=" << bitsToHex(C) << " D=" << bitsToHex(D) << " -> K" << (round + 1) << "="
                      << bitsToHex(roundKey) << "\n";
    }
    return ks;
}

// feistel function
inline Bits feistel(const Bits &R, const Bits &roundKey, bool verbose)
{
    Bits expanded = permute(R, E_TABLE, 48);
    Bits xored = xorBits(expanded, roundKey);
    Bits sboxOut;
    sboxOut.reserve(32);
    for (int i = 0; i < 8; i++)
    {
        int row = (xored[i * 6 + 0] << 1) | xored[i * 6 + 5];
        int col = (xored[i * 6 + 1] << 3) | (xored[i * 6 + 2] << 2) | (xored[i * 6 + 3] << 1) | xored[i * 6 + 4];
        int val = S_BOX[i][row][col];
        for (int b = 3; b >= 0; b--)
            sboxOut.push_back((val >> b) & 1);
    }
    Bits result = permute(sboxOut, P_TABLE, 32);
    if (verbose)
        std::cout << "        f(R,K): E(R)=" << bitsToHex(expanded) << " XOR K=" << bitsToHex(xored)
                  << " S-box=" << bitsToHex(sboxOut) << " P()=" << bitsToHex(result) << "\n";
    return result;
}

// encypt-decrypt
inline Bits encryptBlock(const Bits &plainBlock64, const KeySchedule &ks, bool verbose,
                         const std::string &blockLabel = "")
{
    Bits ip = permute(plainBlock64, IP_TABLE, 64);
    Bits L(ip.begin(), ip.begin() + 32);
    Bits R(ip.begin() + 32, ip.end());
    if (verbose)
        std::cout << "    " << blockLabel << " After IP: L0=" << bitsToHex(L) << " R0=" << bitsToHex(R) << "\n";

    for (int round = 0; round < 16; round++)
    {
        Bits newL = R;
        Bits fout = feistel(R, ks.roundKeys[round], verbose);
        Bits newR = xorBits(L, fout);
        L = newL;
        R = newR;
        if (verbose)
            std::cout << "    " << blockLabel << " Round " << (round + 1) << ": L=" << bitsToHex(L)
                      << " R=" << bitsToHex(R) << "\n";
    }

    Bits preOutput;
    preOutput.reserve(64);
    preOutput.insert(preOutput.end(), R.begin(), R.end());
    preOutput.insert(preOutput.end(), L.begin(), L.end());
    Bits cipher = permute(preOutput, FP_TABLE, 64);
    if (verbose)
        std::cout << "    " << blockLabel << " After swap + FP: cipher=" << bitsToHex(cipher) << "\n";

    return cipher;
}

inline Bits decryptBlock(const Bits &cipherBlock64, const KeySchedule &ks, bool verbose,
                         const std::string &blockLabel = "")
{
    Bits ip = permute(cipherBlock64, IP_TABLE, 64);
    Bits L(ip.begin(), ip.begin() + 32);
    Bits R(ip.begin() + 32, ip.end());
    if (verbose)
        std::cout << "    " << blockLabel << " After IP: L0=" << bitsToHex(L) << " R0=" << bitsToHex(R) << "\n";

    for (int round = 0; round < 16; round++)
    {
        Bits newL = R;
        Bits fout = feistel(R, ks.roundKeys[15 - round], verbose);
        Bits newR = xorBits(L, fout);
        L = newL;
        R = newR;
        if (verbose)
            std::cout << "    " << blockLabel << " Round " << (round + 1) << ": L=" << bitsToHex(L)
                      << " R=" << bitsToHex(R) << "\n";
    }

    Bits preOutput;
    preOutput.reserve(64);
    preOutput.insert(preOutput.end(), R.begin(), R.end());
    preOutput.insert(preOutput.end(), L.begin(), L.end());
    Bits cipher = permute(preOutput, FP_TABLE, 64);
    if (verbose)
        std::cout << "    " << blockLabel << " After swap + FP: cipher=" << bitsToHex(cipher) << "\n";

    return cipher;
}

inline std::vector<uint8_t> hexToBytes(const std::string &hex)
{
    if (hex.size() % 2 != 0)
        return {};

    std::vector<uint8_t> bytes;
    bytes.reserve(hex.size() / 2);

    auto hexValue = [](char c) -> int
    {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };

    for (size_t i = 0; i < hex.size(); i += 2)
    {
        int high = hexValue(hex[i]);
        int low = hexValue(hex[i + 1]);

        if (high < 0 || low < 0)
            return {};

        bytes.push_back(static_cast<uint8_t>((high << 4) | low));
    }

    return bytes;
}

inline Bits hexToBits(const std::string &hex)
{
    std::vector<uint8_t> bytes = hexToBytes(hex);
    if (bytes.empty() && !hex.empty())
        return {};

    return bytesToBits(bytes);
}

inline std::string encryptMessage(const std::string &plaintext,
                                  const std::string &keyHex,
                                  bool verbose = false)
{
    // DES key harus 64 bit = 16 hexadecimal characters.
    if (keyHex.size() != 16)
        return "";

    Bits key64 = hexToBits(keyHex);
    if (key64.size() != 64)
        return "";

    KeySchedule ks = generateRoundKeys(key64, verbose);

    // PKCS#7: selalu tambahkan padding.
    const size_t blockSize = 8;
    size_t padding = blockSize - (plaintext.size() % blockSize);

    std::vector<uint8_t> padded(plaintext.begin(), plaintext.end());
    padded.insert(padded.end(), padding, static_cast<uint8_t>(padding));

    std::string ciphertext;

    for (size_t offset = 0; offset < padded.size(); offset += blockSize)
    {
        std::vector<uint8_t> blockBytes(
            padded.begin() + offset,
            padded.begin() + offset + blockSize
        );

        Bits block64 = bytesToBits(blockBytes);

        Bits encrypted = encryptBlock(
            block64,
            ks,
            verbose,
            "Block " + std::to_string(offset / blockSize)
        );

        ciphertext += bitsToHex(encrypted);
    }

    return ciphertext;
}

inline std::string decryptMessage(const std::string &ciphertextHex,
                                  const std::string &keyHex,
                                  bool verbose = false)
{
    if (keyHex.size() != 16)
        return "";

    // Satu ciphertext block DES = 64 bit = 16 hex characters.
    if (ciphertextHex.empty() || ciphertextHex.size() % 16 != 0)
        return "";

    Bits key64 = hexToBits(keyHex);
    if (key64.size() != 64)
        return "";

    KeySchedule ks = generateRoundKeys(key64, verbose);

    std::vector<uint8_t> paddedPlaintext;

    for (size_t offset = 0; offset < ciphertextHex.size(); offset += 16)
    {
        std::string blockHex = ciphertextHex.substr(offset, 16);
        Bits block64 = hexToBits(blockHex);

        if (block64.size() != 64)
            return "";

        Bits decrypted = decryptBlock(
            block64,
            ks,
            verbose,
            "Block " + std::to_string(offset / 16)
        );

        std::vector<uint8_t> blockBytes = bitsToBytes(decrypted);
        paddedPlaintext.insert(
            paddedPlaintext.end(),
            blockBytes.begin(),
            blockBytes.end()
        );
    }

    if (paddedPlaintext.empty())
        return "";

    uint8_t padding = paddedPlaintext.back();

    if (padding == 0 || padding > 8 || padding > paddedPlaintext.size())
        return "";

    for (size_t i = paddedPlaintext.size() - padding;
         i < paddedPlaintext.size();
         ++i)
    {
        if (paddedPlaintext[i] != padding)
            return "";
    }

    paddedPlaintext.resize(paddedPlaintext.size() - padding);

    return std::string(
        paddedPlaintext.begin(),
        paddedPlaintext.end()
    );
}

} // namespace des