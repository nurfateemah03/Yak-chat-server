// Sentry.h

#ifndef SENTRY_H
#define SENTRY_H

#include "Object.h" // Ensure this is included if Object is in a different file

namespace df {

    class Sentry : public Object { // Ensure Sentry inherits from Object
    public:
        Sentry(); // Constructor
        int eventHandler(const Event* p_e); // Event handler
        void handleStep(); // Handle step events
    };

}

#endif // SENTRY_H
