#include <iostream>
#include <thread>
#include <cstring>
#include "Client.h"
#include "GameManager.h"
#include "TextBox.h"
#include "Input.h"
#include "LogManager.h"
#include "NameEntry.h"
#include <thread>

// Function prototypes.
void populateWorld();
void receiveMessages(Client* client);  // Function to receive messages
#define MAX_MESSAGE_LENGTH 1024
char buffer[MAX_MESSAGE_LENGTH];
void populateWorld();

int main(int argc, char* argv[]) {
    if (GM.startUp()) {
        LM.writeLog("Error starting game manager!");
        GM.shutDown();
        return 0;
    }

    LM.setLogLevel(1);
    LM.setFlush(true);
    LM.writeLog("Chat (v%.1f)", 1.0);

    Client client;
    const char* server_host = "localhost";  
    int server_port = 6000;                

    if (client.connectToServer(server_host, server_port) == 0) {
        LM.writeLog("Connected to server.");

        df::TextBox* p_text_box = new df::TextBox(); // Create TextBox
        Input* input = new Input("Player", p_text_box, &client);  // Pass client to Input
        client.startListening(input); // Start listening for incoming messages

        df::splash(); // Splash screen

        populateWorld(); // Create initial game objects

        GM.run(); // Run game loop

        client.closeConnection(); // Close client connection
    }
    else {
        LM.writeLog("Failed to connect to server.");
    }

    GM.shutDown();
    return 0;
}

// Function that continuously listens for messages from the server.
void receiveMessages(Client* client) {
    char buffer[MAX_MESSAGE_LENGTH];
    while (true) {
        int received = client->receiveMessage(buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';  // Null-terminate the received data
            std::cout << "Message from server: " << buffer << std::endl;
        }
        else {
            break;  // Connection closed or error occurred
        }
    }
}

void populateWorld() {
    // Get name. NameEntry spawns others upon callback.
    NameEntry* p_n = new NameEntry();
    if (!p_n) {
        LM.writeLog("main(): Error! Cannot allocate NameEntry.");
        exit(-1);
    }
}
