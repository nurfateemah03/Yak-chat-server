#pragma once
// Message.h

#ifndef MESSAGE_H
#define MESSAGE_H

enum class MessageType {
    UNDEFINED = -1,
    EXIT,
    CHAT,
};

struct Message {
    int size;           // Total size of the message (including size and type)
    MessageType type;   // Type of the message
    char data[1024];    
};

#endif // MESSAGE_H
