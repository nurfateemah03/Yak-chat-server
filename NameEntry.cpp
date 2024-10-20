//
// NameEntry.cpp
//

// Engine includes.
#include "Color.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"

// Game includes.
#include "Input.h"
#include "NameEntry.h"
#include "TextBox.h"
#include "Client.h"

NameEntry::NameEntry() {
    setType("NameEntry");
    setText("");
    setViewString("enter your name: ");
    setBorder(true);
    setColor(df::WHITE);
    setLocation(df::CENTER_CENTER);
    setLimit(20); // In characters
}

// On callback, start chatting.
void NameEntry::callback() {
    LM.writeLog("NameEntry::callback(): name: %s", getText().c_str());

    // Chat input.
    df::TextBox* p_t = new df::TextBox();
    if (!p_t) {
        LM.writeLog("populateWorld(): Error! Cannot allocate TextBox.");
        exit(-1);
    }
    p_t->setSize(df::Vector(40, 15));

    // Message input. Pass the global or singleton Client instance here.
    new Input(getText(), p_t, &globalClient);

    // Done.
    WM.markForDelete(this);
}