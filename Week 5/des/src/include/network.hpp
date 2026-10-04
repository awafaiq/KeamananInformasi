// network.hpp
// -----------------------------------------------------------------------
// Minimal TCP socket helpers so sender.cpp/receiver.cpp stay focused on DES.
#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace net
{

#ifdef _WIN32
using Socket = SOCKET;
using SocketLength = int;
constexpr Socket InvalidSocket = INVALID_SOCKET;

inline void initializeSockets()
{
    static const bool initialized = [] {
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
            throw std::runtime_error("Failed to initialize Winsock");
        return true;
    }();
    (void)initialized;
}

inline void closeSocket(Socket sock) { closesocket(sock); }
#else
using Socket = int;
using SocketLength = socklen_t;
constexpr Socket InvalidSocket = -1;

inline void initializeSockets() {}
inline void closeSocket(Socket sock) { close(sock); }
#endif

inline Socket createServerSocket(int port)
{
    initializeSockets();
    Socket sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == InvalidSocket)
        throw std::runtime_error("Failed to create socket");

    int opt = 1;
#ifdef _WIN32
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&opt), sizeof(opt));
#else
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // listen on all local interfaces
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        closeSocket(sockfd);
        throw std::runtime_error("Bind failed - port may already be in use");
    }
    if (listen(sockfd, 1) < 0)
    {
        closeSocket(sockfd);
        throw std::runtime_error("Listen failed");
    }
    return sockfd;
}

inline Socket acceptClient(Socket serverSock, std::string &clientIP)
{
    sockaddr_in clientAddr{};
    SocketLength len = sizeof(clientAddr);
    Socket clientSock = accept(serverSock, reinterpret_cast<sockaddr *>(&clientAddr), &len);
    if (clientSock == InvalidSocket)
        throw std::runtime_error("Accept failed");
    clientIP = inet_ntoa(clientAddr.sin_addr);
    return clientSock;
}

inline Socket connectToServer(const std::string &ip, int port)
{
    initializeSockets();
    Socket sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == InvalidSocket)
        throw std::runtime_error("Failed to create socket");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
#ifdef _WIN32
    SocketLength addrLen = sizeof(addr);
    int addressResult = WSAStringToAddressA(
        const_cast<char *>(ip.c_str()), AF_INET, nullptr,
        reinterpret_cast<sockaddr *>(&addr), &addrLen);
#else
    int addressResult = inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
#endif
    if (addressResult <= 0)
    {
        closeSocket(sockfd);
        throw std::runtime_error("Invalid IP address: " + ip);
    }
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        closeSocket(sockfd);
        throw std::runtime_error("Connection failed - is the receiver running and reachable?");
    }
    return sockfd;
}

inline void sendAll(Socket sock, const void *data, size_t len)
{
    const char *ptr = static_cast<const char *>(data);
    size_t sent = 0;
    while (sent < len)
    {
    #ifdef _WIN32
        int chunkSize = static_cast<int>(std::min(len - sent, static_cast<size_t>(std::numeric_limits<int>::max())));
        int n = send(sock, ptr + sent, chunkSize, 0);
    #else
        ssize_t n = send(sock, ptr + sent, len - sent, 0);
    #endif
        if (n <= 0)
            throw std::runtime_error("Send failed / connection lost");
        sent += static_cast<size_t>(n);
    }
}

inline void recvAll(Socket sock, void *data, size_t len)
{
    char *ptr = static_cast<char *>(data);
    size_t received = 0;
    while (received < len)
    {
    #ifdef _WIN32
        int chunkSize = static_cast<int>(std::min(len - received, static_cast<size_t>(std::numeric_limits<int>::max())));
        int n = recv(sock, ptr + received, chunkSize, 0);
    #else
        ssize_t n = recv(sock, ptr + received, len - received, 0);
    #endif
        if (n <= 0)
            throw std::runtime_error("Connection closed unexpectedly / recv failed");
        received += static_cast<size_t>(n);
    }
}

} // namespace net
