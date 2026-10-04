// message_protocol.hpp
// -----------------------------------------------------------------------
// One persistent TCP connection is used for the whole chat session.
// Each message sent over it (in either direction) is framed as:
//
//     [4-byte block count, network order][8-byte IV][ciphertext bytes]
//
// A block count of 0 is a reserved sentinel meaning "I'm ending the
// conversation" (a real message always PKCS#7-pads to at least one
// block, so 0 never collides with a real message).
// -----------------------------------------------------------------------
#pragma once
#include <cctype>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "crypto_utils.hpp"
#include "des.hpp"
#include "network.hpp"

namespace chat
{

struct DecryptionError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

inline void printHex(const std::vector<uint8_t> &bytes)
{
    for (uint8_t b : bytes)
        printf("%02X ", b);
}

inline bool isExitCommand(const std::string &s)
{
    std::string t;
    for (char c : s)
        if (!isspace(static_cast<unsigned char>(c)))
            t += static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return t == "exit";
}

inline void sendExitSignal(net::Socket sock)
{
    uint32_t zero = htonl(0);
    net::sendAll(sock, &zero, sizeof(zero));
}

inline void sendEncryptedMessage(net::Socket sock, const des::KeySchedule &ks, const std::string &message, int turnNumber)
{
    std::vector<uint8_t> plainBytes(message.begin(), message.end());
    std::vector<uint8_t> padded = des_util::pkcs7Pad(plainBytes);
    int numBlocks = static_cast<int>(padded.size() / des_util::BLOCK_SIZE);
    std::vector<uint8_t> iv = des_util::generateRandomIV();

    std::cout << "\n>>> [Turn " << turnNumber << "] Encrypting outgoing message: \"" << message << "\" ("
              << plainBytes.size() << " bytes, padded to " << padded.size() << " -> " << numBlocks << " block(s))\n";
    std::cout << "    Random IV: ";
    printHex(iv);
    std::cout << "\n";

    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> prevBlock = iv;

    for (int i = 0; i < numBlocks; ++i)
    {
        std::vector<uint8_t> block(padded.begin() + i * 8, padded.begin() + i * 8 + 8);
        std::vector<uint8_t> xoredIn = des_util::xorBytes(block, prevBlock);

        std::cout << "  --- Block " << (i + 1) << "/" << numBlocks << " ---\n";
        std::cout << "    Plaintext block      : ";
        printHex(block);
        std::cout << "\n    XOR with prev cipher : ";
        printHex(prevBlock);
        std::cout << "\n    -> Input to DES      : ";
        printHex(xoredIn);
        std::cout << "\n";

        des::Bits inputBits = des::bytesToBits(xoredIn);
        des::Bits cipherBits = des::encryptBlock(
            inputBits, ks, true, "[T" + std::to_string(turnNumber) + " Block " + std::to_string(i + 1) + "]");
        std::vector<uint8_t> cipherBlock = des::bitsToBytes(cipherBits);

        std::cout << "    Cipher block         : ";
        printHex(cipherBlock);
        std::cout << "\n";

        ciphertext.insert(ciphertext.end(), cipherBlock.begin(), cipherBlock.end());
        prevBlock = cipherBlock;
    }

    std::cout << "    Final ciphertext: ";
    for (uint8_t b : ciphertext)
        printf("%02X", b);
    std::cout << "\n";

    uint32_t nBlocksNet = htonl(static_cast<uint32_t>(numBlocks));
    net::sendAll(sock, &nBlocksNet, sizeof(nBlocksNet));
    net::sendAll(sock, iv.data(), iv.size());
    net::sendAll(sock, ciphertext.data(), ciphertext.size());
}

struct ReceivedMessage
{
    bool isExit;
    std::string plaintext;
};

inline ReceivedMessage receiveEncryptedMessage(net::Socket sock, const des::KeySchedule &ks, int turnNumber)
{
    uint32_t nBlocksNet;
    net::recvAll(sock, &nBlocksNet, sizeof(nBlocksNet));
    int numBlocks = static_cast<int>(ntohl(nBlocksNet));

    if (numBlocks == 0)
    {
        return ReceivedMessage{true, ""};
    }

    std::vector<uint8_t> iv(des_util::BLOCK_SIZE);
    net::recvAll(sock, iv.data(), iv.size());
    std::vector<uint8_t> ciphertext(numBlocks * des_util::BLOCK_SIZE);
    net::recvAll(sock, ciphertext.data(), ciphertext.size());

    std::cout << "\n<<< [Turn " << turnNumber << "] Received " << numBlocks << " block(s), decrypting:\n";
    std::cout << "    Received IV: ";
    printHex(iv);
    std::cout << "\n    Received ciphertext: ";
    for (uint8_t b : ciphertext)
        printf("%02X", b);
    std::cout << "\n";

    std::vector<uint8_t> plainPadded;
    std::vector<uint8_t> prevBlock = iv;

    for (int i = 0; i < numBlocks; ++i)
    {
        std::vector<uint8_t> cblock(ciphertext.begin() + i * 8, ciphertext.begin() + i * 8 + 8);

        std::cout << "  --- Block " << (i + 1) << "/" << numBlocks << " ---\n";
        std::cout << "    Cipher block         : ";
        printHex(cblock);
        std::cout << "\n";

        des::Bits cipherBits = des::bytesToBits(cblock);
        des::Bits plainBits = des::decryptBlock(
            cipherBits, ks, true, "[T" + std::to_string(turnNumber) + " Block " + std::to_string(i + 1) + "]");
        std::vector<uint8_t> desOutput = des::bitsToBytes(plainBits);
        std::vector<uint8_t> plainBlock = des_util::xorBytes(desOutput, prevBlock);

        std::cout << "    DES output           : ";
        printHex(desOutput);
        std::cout << "\n    XOR with prev cipher : ";
        printHex(prevBlock);
        std::cout << "\n    -> Plaintext block   : ";
        printHex(plainBlock);
        std::cout << " (\"";
        for (uint8_t b : plainBlock)
            std::cout << (isprint(b) ? static_cast<char>(b) : '.');
        std::cout << "\")\n";

        plainPadded.insert(plainPadded.end(), plainBlock.begin(), plainBlock.end());
        prevBlock = cblock;
    }

    std::vector<uint8_t> plain;
    try
    {
        plain = des_util::pkcs7Unpad(plainPadded);
    }
    catch (const std::exception &)
    {
        throw DecryptionError(
            "Decryption failed: padding is invalid. The shared keys on the two sides probably don't match (or the data was corrupted).");
    }
    std::string message(plain.begin(), plain.end());
    return ReceivedMessage{false, message};
}

} // namespace chat
