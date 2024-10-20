#pragma once
//
// Input.h - Uses TextEntry to get chat input.
//

// System includes.
#include <string>

// Engine includes.
#include "TextBox.h"
#include "TextEntry.h"

class Client;

class Input : public df::TextEntry {

private:
    df::TextBox* m_p_text_box;  // Text box for chat messages.
    std::string m_name;         // Name of chatter.
    Client* m_p_client;         
    df::TextBox* textBox; 


public:
    Input(std::string name, df::TextBox* p_text_box, Client* p_client);  // Updated constructor.
    void callback() override;
    void updateUIWithMessage(const char* message);

};
