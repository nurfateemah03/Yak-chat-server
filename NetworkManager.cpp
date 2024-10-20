#include "NetworkManager.h"
#include <iostream>
#include "LogManager.h"
#include "EventNetwork.h"
#include "WorldManager.h"
//comment

namespace df {

    // Constructor for NetworkManager
    NetworkManager::NetworkManager() {
        m_accept_sock = INVALID_SOCKET;  // Initialize the accept socket
        m_sock.clear();                  // Clear any existing sockets
        server_mode = false;             // Initialize the server mode

        df::LogManager& lm = df::LogManager::getInstance();
        lm.writeLog("NetworkManager initialized.");
    }

    NetworkManager& NetworkManager::getInstance() {
        static NetworkManager instance;  
        return instance;
    }

    // Start up NetworkManager.
    int NetworkManager::startUp() {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return -1;
        }
        return Manager::startUp();
    }

    // Shut down NetworkManager.
    void NetworkManager::shutDown() {
        if (m_accept_sock != INVALID_SOCKET) {
            closesocket(m_accept_sock);
            m_accept_sock = INVALID_SOCKET;
        }

        closeAll();  // Call closeAll to close all sockets.
        WSACleanup();
        Manager::shutDown();
    }

    // Close all active connections.
    int NetworkManager::closeAll() {
        for (int sock : m_sock) {
            if (sock != INVALID_SOCKET) {
                closesocket(sock);  // Close the socket
            }
        }
        m_sock.clear();  // Clear the socket list
        std::cout << "All connections closed." << std::endl;
        return 0;
    }

    // Return the number of active connections.
    int NetworkManager::getNumConnections() const {
        return m_sock.size();
    }

    // Receive data from the specified socket.
    int NetworkManager::receive(void* buffer, int length, bool peek, int sock_index) {
        if (sock_index < 0 || sock_index >= m_sock.size()) {
            return -1;  // Invalid socket index.
        }

        int sock = m_sock[sock_index];
        int flags = 0;
        if (peek) {
            flags = MSG_PEEK;  // Option to peek without removing data from queue.
        }

        int result = recv(sock, (char*)buffer, length, flags);
        if (result == SOCKET_ERROR) {
            return -1;  // Error receiving data.
        }

        return result;  // Return the number of bytes received.
    }

    // Setup NetworkManager as server or client.
    int NetworkManager::setServer(bool server, std::string port) {
        server_mode = server;

        if (server_mode) {
            addrinfo hints{}, * server_info;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;  // Use my IP.

            if (getaddrinfo(NULL, port.c_str(), &hints, &server_info) != 0) {
                std::cerr << "getaddrinfo failed." << std::endl;
                return -1;
            }

            m_accept_sock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
            if (m_accept_sock == INVALID_SOCKET) {
                std::cerr << "Failed to create accept socket." << std::endl;
                freeaddrinfo(server_info);
                return -1;
            }

            if (bind(m_accept_sock, server_info->ai_addr, (int)server_info->ai_addrlen) == -1) {
                std::cerr << "Bind failed." << std::endl;
                closesocket(m_accept_sock);
                freeaddrinfo(server_info);
                return -1;
            }

            if (listen(m_accept_sock, SOMAXCONN) == -1) {
                std::cerr << "Listen failed." << std::endl;
                closesocket(m_accept_sock);
                freeaddrinfo(server_info);
                return -1;
            }

            freeaddrinfo(server_info);
            std::cout << "Server setup successful." << std::endl;
        }
        else {
            if (m_accept_sock != INVALID_SOCKET) {
                closesocket(m_accept_sock);
                m_accept_sock = INVALID_SOCKET;
            }
            std::cout << "Client setup." << std::endl;
        }

        return 0;
    }

    // Return true if successfully setup as server.
    bool NetworkManager::isServer() const {
        return server_mode;
    }

    // Accept network connection.
    int NetworkManager::accept() {
        if (!server_mode || m_accept_sock == INVALID_SOCKET) {
            return -1;
        }

        int new_sock = ::accept(m_accept_sock, NULL, NULL);
        if (new_sock == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                return -1;  // No new connection, not an error.
            }
            return -2;  // Error.
        }

        m_sock.push_back(new_sock);
        return m_sock.size() - 1;  // Return index of new connection.
    }
    // Check if the specified socket is connected.
    bool df::NetworkManager::isConnected(int sock_index) const {
        if (sock_index < 0 || sock_index >= m_sock.size()) {
            return false;  // Invalid socket index.
        }

        int sock = m_sock[sock_index];
        return sock != INVALID_SOCKET;  // Return true if the socket is valid.
    }

    // Check if there is data available on the specified socket.
    int df::NetworkManager::isData(int sock_index) const {
        if (sock_index < 0 || sock_index >= m_sock.size()) {
            return -1;  // Invalid socket index.
        }

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(m_sock[sock_index], &read_fds);

        timeval timeout{};
        timeout.tv_sec = 0;  // No timeout, just poll.
        timeout.tv_usec = 0;

        int result = select(0, &read_fds, NULL, NULL, &timeout);
        if (result == SOCKET_ERROR) {
            return -1;  // Error in select().
        }

        return result > 0 ? 1 : 0;  // Return 1 if data is available, 0 if not.
    }
  
#include "NetworkManager.h"
#include "LogManager.h"

    int df::NetworkManager::send(const void* buffer, int bytes, int sock_index) {
        // Get the system socket from the socket index.
        int sock = getSocket(sock_index);
        if (sock == -1) {
            LM.writeLog("NetworkManager::send: Invalid socket index %d.", sock_index);
            return -1;
        }

        // Send data to the specified socket.
        int sent_bytes = ::send(sock, static_cast<const char*>(buffer), bytes, 0);
        if (sent_bytes == SOCKET_ERROR) {
            LM.writeLog("NetworkManager::send: Error sending data on socket %d.", sock);
            return -1;
        }

        return 0;  // Success.
    }
    int df::NetworkManager::close(int sock_index) {
        // Get the system socket from the socket index.
        int sock = getSocket(sock_index);
        if (sock == -1) {
            LM.writeLog("NetworkManager::close: Invalid socket index %d.", sock_index);
            return -1;
        }

        // Close the socket connection.
        int result = ::closesocket(sock);
        if (result == SOCKET_ERROR) {
            LM.writeLog("NetworkManager::close: Error closing socket %d.", sock);
            return -1;
        }

        // Remove the socket from the list.
        m_sock.erase(m_sock.begin() + sock_index);

        // Generate a close event.
        df::EventNetwork close_event(df::NetworkEventLabel::CLOSE);
        close_event.setSocketIndex(sock_index);
        WM.onEvent(&close_event);

        return 0;  // Success.
    }

    int df::NetworkManager::getSocket(int sock_index) const {
        // Check if the socket index is valid.
        if (sock_index < 0 || sock_index >= m_sock.size()) {
            LM.writeLog("NetworkManager::getSocket: Invalid socket index %d.", sock_index);
            return -1;  // Invalid socket index.
        }

        return m_sock[sock_index];  // Return the socket at the given index.
    }

} 
