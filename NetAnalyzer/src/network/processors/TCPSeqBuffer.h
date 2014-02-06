/*
 * TCPSeqBuffer.h
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#ifndef TCPSEQBUFFER_H_
#define TCPSEQBUFFER_H_

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <vector>


/**
 * represents one buffered segment
 */
struct TCPBufferedSegment {

	/** the sequence number of the segment */
	uint32_t seqNumber;

	/** the data of the segment */
	uint8_t* data;

	/** the length of the segment */
	uint32_t length;



	/** ctor. the provided data* will be COPIED! */
	TCPBufferedSegment(uint32_t seqNumner, const uint8_t* data, uint32_t length) :
		seqNumber(seqNumner), length(length) {

		// create a copy of the given data
		this->data = (uint8_t*) malloc(length);
		memcpy(this->data, data, length);

	}

	/** dtor */
	~TCPBufferedSegment() {

		// remove allocated data
		if (this->data != nullptr) {free(data); data = nullptr;}

	}

};


/**
 * buffer TCP-Packets to re-assemble them by their sequence number
 */
class TCPSeqBuffer {

public:

	/** ctor */
	TCPSeqBuffer();

	/** dtor */
	virtual ~TCPSeqBuffer();

	/**
	 * get a buffered element with the requested seqNumber or nullptr if not found.
	 * the returned element MUST be deleted() !!
	 */
	TCPBufferedSegment* get(uint32_t seqNumber);

	/** append a new segment to the buffer. the data will be COPIED! */
	void append(uint32_t seqNumber, const uint8_t* data, uint32_t length);

	/** the buffer might have a limit which can be checked here */
	bool isFull();

private:

	/** buffer the elements here */
	std::vector<TCPBufferedSegment*> buffer;


};

#endif /* TCPSEQBUFFER_H_ */
