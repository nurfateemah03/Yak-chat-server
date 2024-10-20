#ifndef CLIENT_H
#define CLIENT_H

#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#include "Input.h"
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define MAX_MESSAGE_LENGTH 1024

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

class Client {
private:
    int sock;
    bool server_connected;
    void listenForMessages(Input* input); // Method to handle incoming messages


public:
    // Constructor and Destructor
    Client();
    ~Client();

    // Connect to server with hostname and port number
    int connectToServer(const char* server_host, int server_port);

    // Send a message to the server
    int sendMessage(const char* message);

    // Receive a message from the server
    int receiveMessage(char* buffer, int length);

    // Close the connection
    void closeConnection();

    // Utility function to print network errors
    void print_network_error(const char* message, int error);
    void startListening(Input* input);  // Ensure this matches the definition

};

// Declare the global Client instance
extern Client globalClient;

#endif // CLIENT_H
