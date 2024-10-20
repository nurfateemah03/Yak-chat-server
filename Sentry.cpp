// Sentry.cpp

#include "Sentry.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "EventNetwork.h"

namespace df {

    Sentry::Sentry() {
    }

    // Event handler
    int Sentry::eventHandler(const Event* p_e) {
        if (p_e->getType() == STEP_EVENT) {
            handleStep();
            return 1; // Event handled
        }
        return 0; // Event not handled
    }

    // Handle step events
    void Sentry::handleStep() {
        NetworkManager& nm = NetworkManager::getInstance();

        if (nm.isServer()) {
            int new_conn = nm.accept();
            if (new_conn >= 0) {
                // Generate an EventNetwork for accepted connection
                EventNetwork e(NetworkEventLabel::ACCEPT);
                e.setSocketIndex(new_conn);
                WorldManager::getInstance().onEvent(&e);
            }
        }

        if (nm.getNumConnections() > 0) {
            for (int i = 0; i < nm.getNumConnections(); ++i) {
                if (nm.isConnected(i)) {
                    int bytes_available = nm.isData(i);
                    if (bytes_available >= sizeof(int)) {
                        int msg_size = 0;
                        nm.receive(&msg_size, sizeof(int), true, i);
                        if (nm.isData(i) >= msg_size) {
                            EventNetwork e(NetworkEventLabel::DATA);
                            e.setSocketIndex(i);
                            e.setBytes(msg_size);
                            WorldManager::getInstance().onEvent(&e);
                        }
                    }
                }
            }
        }
    }

}
