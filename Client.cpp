// client.cpp - Client socket handling
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Client.h"
#include "LogManager.h"
#include "TextBox.h"
#include <thread>
Client globalClient;  //

Client::Client() {
    sock = INVALID_SOCKET;
    server_connected = false;
}

Client::~Client() {
    closeConnection();
}

int Client::connectToServer(const char* server_host, int server_port) {
    struct sockaddr_in serv_addr;
    struct hostent* hp;

    // Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        print_network_error("WSAStartup() failed", WSAGetLastError());
        return -1;
    }

    // Lookup server hostname
    hp = gethostbyname(server_host);
    if (hp == NULL) {
        print_network_error("gethostbyname() failed", WSAGetLastError());
        return -1;
    }

    memset((void*)&serv_addr, 0, sizeof(serv_addr));
    memcpy(&serv_addr.sin_addr, hp->h_addr, hp->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // Create socket
    sock = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        print_network_error("socket() creation failed", WSAGetLastError());
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        print_network_error("connect() failed", WSAGetLastError());
        return -1;
    }

    server_connected = true;
    LM.writeLog("Client connected to server.");
    return 0;
}

void Client::closeConnection() {
    if (sock != INVALID_SOCKET) {
#if defined(_WIN32) || defined(_WIN64)
        shutdown(sock, SD_BOTH); // Shut down the connection gracefully
        closesocket(sock);
#else
        shutdown(sock, SHUT_RDWR); // Shut down the connection gracefully
        close(sock);
#endif
        sock = INVALID_SOCKET;
        server_connected = false;
    }

#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif
}


int Client::sendMessage(const char* message) {
    if (server_connected) {
        if (send(sock, message, (unsigned int)strlen(message), 0) == SOCKET_ERROR) {
            print_network_error("send() error", WSAGetLastError());
            return -1;
        }
        return 0;
    }
    return -1;
}

int Client::receiveMessage(char* buffer, int length) {
    if (server_connected) {
        int result = recv(sock, buffer, length, 0);
        if (result == SOCKET_ERROR) {
            print_network_error("recv() error", WSAGetLastError());
            return -1;
        }
        return result;
    }
    return -1;
}

void Client::print_network_error(const char* message, int error) {
    LPTSTR error_text = NULL;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error_text, 0, NULL);
    if (error_text != NULL) {
        fprintf(stderr, "%s: %d - %S", message, error, error_text);
        LocalFree(error_text);
        error_text = NULL;
    }
}
void Client::listenForMessages(Input* input) {
    char buffer[MAX_MESSAGE_LENGTH];
    int bytesReceived;

    while (server_connected) {
        bytesReceived = receiveMessage(buffer, sizeof(buffer) - 1);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            input->updateUIWithMessage(buffer); // Update UI with the new message
        }
    }
}
void Client::startListening(Input* input) {
    std::thread([this, input]() {
        char buffer[MAX_MESSAGE_LENGTH];
        while (server_connected) {
            int bytes_received = receiveMessage(buffer, sizeof(buffer));
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Null-terminate
                LM.writeLog("Received message: %s", buffer);

                // Update the UI with the received message
                if (input) {
                    input->updateUIWithMessage(buffer); // Call method on Input
                }
            }
        }
        }).detach();
}

