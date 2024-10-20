#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#define PORT 6000
#define MAX_MESSAGE_LENGTH 1024

std::vector<int> client_sockets;
std::mutex client_mutex;

void broadcastMessage(const char* message, int sender_sock) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (int sock : client_sockets) {
        send(sock, message, strlen(message), 0);
    }
}

void handleClient(int client_sock) {
    char message[MAX_MESSAGE_LENGTH];
    int bytes;

    while ((bytes = recv(client_sock, message, sizeof(message) - 1, 0)) > 0) {
        message[bytes] = '\0';  // Null-terminate the received data

        // Server displays the message.
        printf("Received from client %d: '%s'\n", client_sock, message);

        // Broadcast the message to all clients.
        broadcastMessage(message, client_sock);
    }

    if (bytes == -1) {
        perror("recv() error");
    }
    else {
        printf("Client %d disconnected.\n", client_sock);
    }

    // Remove client from the list
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_sock), client_sockets.end());
    }

    // Close the client socket
#if defined(_WIN32) || defined(_WIN64)
    closesocket(client_sock);
#else
    close(client_sock);
#endif
}


#include <csignal>

volatile std::sig_atomic_t stop;

void signalHandler(int signum) {
    stop = 1;
}

int main() {
    std::signal(SIGINT, signalHandler); // Register signal handler for graceful shutdown

#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup() failed\n";
        return -1;
    }
#endif

    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket() failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() failed");
        return -1;
    }

    if (listen(server_sock, 5) == -1) {
        perror("listen() failed");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (!stop) {
        int new_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (new_sock == -1) {
            if (stop) break; // Exit if shutdown signal was received
            perror("accept() failed");
            continue;
        }

        std::lock_guard<std::mutex> lock(client_mutex);
        client_sockets.push_back(new_sock);

        std::cout << "New client connected: " << new_sock << "\n";

        std::thread(handleClient, new_sock).detach();
    }

    std::cout << "Server shutting down...\n";

    // Close all client sockets
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        for (int sock : client_sockets) {
#if defined(_WIN32) || defined(_WIN64)
            closesocket(sock);
#else
            close(sock);
#endif
        }
        client_sockets.clear();
    }

#if defined(_WIN32) || defined(_WIN64)
    closesocket(server_sock);
    WSACleanup();
#else
    close(server_sock);
#endif

    return 0;
}

