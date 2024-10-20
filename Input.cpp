//
// Input.cpp
//

// Engine includes.
#include "Color.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "Client.h"

// Game includes.
#include "Input.h"

Input::Input(std::string name, df::TextBox* p_text_box, Client* p_client) {
    setType("Input");
    setText("");
    setBorder(true);
    setColor(df::WHITE);
    setLocation(df::BOTTOM_CENTER);
    setLimit(40); // In characters
    m_p_text_box = p_text_box;
    m_name = name + ": ";
    setViewString(m_name);
    m_p_client = p_client;  // Initialize the client pointer
}
void Input::updateUIWithMessage(const char* message) {
    if (textBox) {
        std::string updatedText = textBox->getText(); 
        updatedText += "\n";
        updatedText += message;
        textBox->setText(updatedText.c_str()); 
    }
}


void Input::callback() {
    LM.writeLog("Input::callback(): Input: %s", getText().c_str());

    if (getText() == "quit" || getText() == "exit") {
        GM.setGameOver(); // Mark game as over
        m_p_client->closeConnection(); // Close connection
    }
    else {
        if (m_p_client && m_p_client->sendMessage(getText().c_str()) == 0) {
            LM.writeLog("Message sent to server: %s", getText().c_str());
        }
        else {
            LM.writeLog("Failed to send message to server.");
        }

        m_p_text_box->addText(m_name);
        m_p_text_box->addText(getText());
        m_p_text_box->addText("\n");

        this->clearText();
    }
}

