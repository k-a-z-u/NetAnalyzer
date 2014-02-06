/*
 * LZWDictStream.h
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#ifndef LZWDICTSTREAM_H_
#define LZWDICTSTREAM_H_

#include <cstdint>
#include <cstdlib>


class LZWDictStream {

public:

	/** ctor */
	inline LZWDictStream(uint32_t size) {
		buffer = (uint16_t*) malloc(size/2);
		tail = buffer;
	}

	/** dtor */
	inline ~LZWDictStream() {
		if (buffer != nullptr) {free(buffer); buffer = nullptr;}
	}

	/** append 2 bytes of data */
	inline void append(uint32_t data) {

		*tail = (uint16_t) data;
		++tail;

//		// add 2 bytes
//		tail[0] = data >> 8;
//		tail[1] = data >> 0;
//
//		// increment
//		tail += 2;

	}

	/** get number of used bytes */
	inline uint32_t getNumUsed() {
		return (tail - buffer) * 2;
	}

	/** get access to the stored data */
	inline const uint8_t* getData() const {
		return (uint8_t*) buffer;
	}

	/** "clear" the buffer */
	inline void clear() {
		tail = buffer;
	}

private:

	/** store data here */
	uint16_t* buffer;

	/** write to this pointer */
	uint16_t* tail;

};

#endif /* LZWDICTSTREAM_H_ */
