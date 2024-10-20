#pragma once
// Server.h

#ifndef SERVER_H
#define SERVER_H

#include "Object.h"
#include "Event.h"
#include "EventNetwork.h"
#include "Message.h"

class Server : public df::Object {
public:
    Server();
    int eventHandler(const df::Event* p_e) override;

private:
    void handleNetworkEvent(const df::EventNetwork* p_ne);
    void broadcastMessage(int sender_socket_index, const Message& msg);
};

#endif // SERVER_H
