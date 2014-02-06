/*
 * TCPSeqBuffer.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#include "TCPSeqBuffer.h"
#include "../../Debug.h"

#define TCP_SEQ_BUFFER_SIZE		64
#define DBG_TCP_BUF				"TCPBuf"


TCPSeqBuffer::TCPSeqBuffer() {

}

TCPSeqBuffer::~TCPSeqBuffer() {

	// warning if buffer is not empty
	if (!buffer.empty()) {
		debug(DBG_TCP_BUF, DBG_LVL_WARN, "had to remove " << buffer.size() << " dangling entries.");
	}

	// delete all still available entries in the buffer
	for (const auto it : buffer) {delete it;}

}

bool TCPSeqBuffer::isFull() {

	// allow max TCP_SEQ_BUFFER_SIZE entries in the buffer
	return buffer.size() >= TCP_SEQ_BUFFER_SIZE;

}

TCPBufferedSegment* TCPSeqBuffer::get(uint32_t seqNumber) {

	// skip immediately o
	if (buffer.empty()) {return nullptr;}

	std::vector<TCPBufferedSegment*>::iterator it = buffer.begin();

	// check if the buffer contains an element for this sequence number
	while (it != buffer.end()) {

		if ((*it)->seqNumber == seqNumber) {
			auto ret = *it;
			buffer.erase(it);
			return ret;
		}
		++it;
	}

	// not found -> return nullptr
	return nullptr;

}

void TCPSeqBuffer::append(uint32_t seqNumber, const uint8_t* data, uint32_t length) {

	// create a new buffer-entry (which will COPY the given data)
	TCPBufferedSegment* seg = new TCPBufferedSegment(seqNumber, data, length);

	// insert into the buffer
	buffer.push_back(seg);

}
