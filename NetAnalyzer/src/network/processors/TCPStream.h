/*
 * TCPStream.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef TCPSTREAM_H_
#define TCPSTREAM_H_

#include "TCPSeqBuffer.h"

#include <set>
#include <cstdint>
#include <cstring>

class TCPStreamListener;

/**
 * details for one direction of a TCP-Stream
 */
struct TCPStreamDirection {

	/** the next expected seq-numer. used to ensure correct packet order */
	uint32_t nextSeqNumber;

	/** buffer segments that were received in a different order here */
	TCPSeqBuffer buffer;

	/** is this direction incoming or outgoing? */
	bool isFromInitiator;

	/** ctor */
	TCPStreamDirection(bool isIncoming) : nextSeqNumber(0), isFromInitiator(isIncoming) {;}

};

/**
 * handle a bi-directional TCP-Stream here
 */
struct TCPStream {

	/** details for incoming packets */
	struct TCPStreamDirection fromInitiator;

	/** details for outgoing packets */
	struct TCPStreamDirection toInitiator;


	/** we store the local-port here to distinguish between incoming and outgoing data */
	uint16_t localPort;

	/** the listener to inform about the data-flow on this stream */
	TCPStreamListener* listener;

	/** the ID this stream is stored under. (needed to erase the stream from the map) */
	uint64_t id;

	/** timestamp of the last activity */
	uint64_t lastActivityTs;

	/** track number of received FIN packets */
	uint32_t numFIN;


	/** ctor */
	TCPStream() :
		fromInitiator(true), toInitiator(false), localPort(0),
		listener(nullptr), id(0), lastActivityTs(0), numFIN(0) {
		;
	}

	/** dtor */
	~TCPStream() {;}

};


#endif /* TCPSTREAM_H_ */
