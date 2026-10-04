#include <exception>
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
    cout << "            DES CLIENT\n";
    cout << "=====================================\n\n";

    // ip and port
    string ip;
    cout << "Enter receiver's IP address: ";
    getline(cin, ip);

    int port;
    cout << "Enter receiver's port: ";
    cin >> port;
    cin.ignore();

    // key
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

    // message over tcp
    net::Socket sock;
    try
    {
        cout << "\n[Setup] Connecting to " << ip << ":" << port << " ...\n";
        sock = net::connectToServer(ip, port);
        cout << "[Setup] Connected. Type messages below - 'exit' ends "
                "the session for both sides.\n";
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
        cout << "Enter message (or 'exit' to end): ";
        string message;
        getline(cin, message);

        if (chat::isExitCommand(message))
        {
            chat::sendExitSignal(sock);
            cout << "\n[Session] You ended the conversation.\n";
            break;
        }

        ++turnNumber;
        chat::sendEncryptedMessage(sock, ks, message, turnNumber);

        cout << "\n[Session] Waiting for receiver's reply...\n";
        try
        {
            chat::ReceivedMessage reply = chat::receiveEncryptedMessage(sock, ks, turnNumber);
            if (reply.isExit)
            {
                cout << "\n[Session] Receiver ended the conversation.\n";
                break;
            }
            cout << "\n[Session] Receiver replied: \"" << reply.plaintext << "\"\n";
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
    }

    net::closeSocket(sock);
    cout << "\n=====================================\n";
    cout << "               DONE\n";
    cout << "=====================================\n";
    return 0;
}