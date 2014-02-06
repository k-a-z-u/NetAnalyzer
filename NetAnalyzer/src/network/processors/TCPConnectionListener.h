/*
 * TCPConnectionListener.h
 *
 * listen for accepted TCP-Connections
 *
 *  Created on: Apr 10, 2013
 *      Author: kazu
 */

#ifndef TCPCONNECTIONLISTENER_H_
#define TCPCONNECTIONLISTENER_H_

#include <cstdint>

/**
 * listen for accepted TCP-Connections
 */
class TCPConnectionListener {

public:

	virtual ~TCPConnectionListener() {;}

	/**
	 * called when this host accepted a TCP connection on the given port.
	 * the listener MUST return a new TCPStreamListener that will receive the data-flow of this connection.
	 * the TCP-System will destroy the TCPStreamListener when the TCP connection is closed
	 */
	virtual TCPStreamListener* onTCPAccepted(uint16_t port) = 0;

};


#endif /* TCPCONNECTIONLISTENER_H_ */
