#include <cstdint>
#include <exception>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "include/des.hpp"
#include "include/message_protocol.hpp"
#include "include/network.hpp"

using namespace std;

int main()
{
    cout << "=====================================\n";
    cout << "             DES SERVER\n";
    cout << "=====================================\n\n";

    int port;
    cout << "Enter port to listen on: ";
    cin >> port;
    cin.ignore();

    string key;
    cout << "Enter shared DES key: ";
    getline(cin, key);

    vector<uint8_t> keyBytes(8, 0x00);
    for (size_t i = 0; i < key.size(); ++i)
    {
        keyBytes[i % 8] ^= static_cast<uint8_t>(key[i]);
    }

    des::Bits keyBits = des::bytesToBits(keyBytes);

    cout << "\n[1] Deriving 16 round keys (K1..K16) from the shared key...\n";
    des::KeySchedule ks = des::generateRoundKeys(keyBits, /*verbose=*/true);

    net::Socket serverSock, sock;
    try
    {
        cout << "\n[Setup] Listening on port " << port << " ...\n";
        serverSock = net::createServerSocket(port);
        string clientIP;
        sock = net::acceptClient(serverSock, clientIP);
        cout << "[Setup] Connection established from " << clientIP << ". 'exit' ends the session for both sides.\n";
    }
    catch (const exception &e)
    {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    int turnNumber = 0;
    while (true)
    {
        cout << "\n---------------------------------------\n";
        cout << "[Session] Waiting for sender's message...\n";
        ++turnNumber;
        try
        {
            chat::ReceivedMessage incoming = chat::receiveEncryptedMessage(sock, ks, turnNumber);
            if (incoming.isExit)
            {
                cout << "\n[Session] Sender ended the conversation.\n";
                break;
            }
            cout << "\n[Session] Sender says: \"" << incoming.plaintext << "\"\n";
        }
        catch (const chat::DecryptionError &e)
        {
            cout << "\n[Session] Could not decrypt the incoming message: " << e.what() << "\n";
            cout << "Check that both sides entered the exact same key.\n";
            break;
        }
        catch (const exception &e)
        {
            cout << "\n[Session] Connection closed unexpectedly (" << e.what() << ").\n";
            break;
        }

        cout << "\nEnter reply (or 'exit' to end): ";
        string message;
        getline(cin, message);

        if (chat::isExitCommand(message))
        {
            chat::sendExitSignal(sock);
            cout << "\n[Session] You ended the conversation.\n";
            break;
        }

        chat::sendEncryptedMessage(sock, ks, message, turnNumber);
    }

    net::closeSocket(sock);
    net::closeSocket(serverSock);
    cout << "\n=====================================\n";
    cout << "               DONE\n";
    cout << "=====================================\n";
    return 0;
}