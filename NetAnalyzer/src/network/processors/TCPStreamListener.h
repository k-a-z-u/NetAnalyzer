/*
 * TCPStreamListener.h
 *
 *  Created on: Apr 10, 2013
 *      Author: kazu
 */

#ifndef TCPSTREAMLISTENER_H_
#define TCPSTREAMLISTENER_H_

struct Payload;

/**
 * listener for TCP-Connection events.
 * the TCP-System will destroy the TCPStreamListener when the TCP connection is closed.
 */
class TCPStreamListener {

public:

	virtual ~TCPStreamListener() {;}

	/** called when connection is opened */
	virtual void onOpen() = 0;

	/** called when connection is closed */
	virtual void onClose() = 0;

	/** called for incoming data on this stream */
	virtual void onDataFromInitiator(const Payload& data) = 0;

	/** called for outgoing data on this stream */
	virtual void onDataToInitiator(const Payload& data) = 0;

};


#endif /* TCPSTREAMLISTENER_H_ */
