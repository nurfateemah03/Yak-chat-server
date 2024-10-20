//
// EventNetwork.cpp - Implementation of EventNetwork class.
//

// System includes.
#include "EventNetwork.h"

namespace df {

	// Constructor - must provide label.
	EventNetwork::EventNetwork(NetworkEventLabel label) {
		setType(NETWORK_EVENT);      // Set event type to df-network.
		m_label = label;             // Set the event label.
		m_socket_index = -1;         
		m_bytes = 0;                 // Default bytes in the message is 0.
	}

	// Set the label of the event.
	void EventNetwork::setLabel(NetworkEventLabel new_label) {
		m_label = new_label;
	}

	// Set the socket index of the event.
	void EventNetwork::setSocketIndex(int new_socket_index) {
		m_socket_index = new_socket_index;
	}

	// Get the socket index of the event.
	int EventNetwork::getSocketIndex() const {
		return m_socket_index;
	}

	// Set the number of bytes in the message.
	void EventNetwork::setBytes(int new_bytes) {
		m_bytes = new_bytes;
	}

	// Get the number of bytes in the message.
	int EventNetwork::getBytes() const {
		return m_bytes;
	}

	NetworkEventLabel EventNetwork::getLabel() const {
		return m_label;
	}

}  // end of namespace df
