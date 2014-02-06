/*
 * BitstreamOut.h
 *
 *  Created on: Apr 30, 2013
 *      Author: kazu
 */

#ifndef BITSTREAMOUT_H_
#define BITSTREAMOUT_H_


#include <cstdint>

#include "../../Debug.h"
#include "opt.h"

/** configuration */
#define BSO_TYPE		size_t
#define BSO_TYPE_BITS	(sizeof(BSO_TYPE) * 8)
#define BSO_SWAP		__builtin_bswap64

/**
 * represents a Bitstream that can be wrapped around an uint8_t*
 * in order to write bits to it.
 */
class BitstreamOut {

public:

	/** wrap this Bitstream around the given array */
	BitstreamOut(uint8_t* data) :
		buffer(data), pending(0), bitPos(0) {

		head = (BSO_TYPE*) buffer;

	}

	/** dtor */
	~BitstreamOut() {;}

	/**
	 * rewint this Bitstream
	 * this will simply reposition the write pointer.
	 * the cache (containing unflushed bits) WON'T be affected!
	 */
	void rewind() {
		head =		(BSO_TYPE*) buffer;
	}

	/**
	 * reset this Bitstream
	 * this will simply reposition the write pointer AND write caches
	 */
	void reset() {
		head =		(BSO_TYPE*) buffer;
		bitPos =	0;
		pending =	0;
	}

	/**
	 * get the number of bytes available for reading.
	 * these value will be a multiply of the element's size
	 * (e.g. on 32-bit systems: 4 and 8 on 64-bit)
	 */
	uint32_t getBytesAvailable() const {
		return (uint8_t*) head - (uint8_t*) buffer;
	}


	void prefetch() {
		//__builtin_prefetch(head+1, 1, 1);
		//__builtin_prefetch(head, 1, 1);
		__builtin_prefetch(&pending, 1, 2);
	}

	/** append the lowest 'numBits' bits of 'value' */
	void append(uint32_t value, uint32_t numBits) {

		// proceed until all bits have been flushed to the buffer
		for( ; ; ) {

			// the current element provides room for this many bits
			uint32_t writeableBits = BSO_TYPE_BITS - bitPos;

			// we need to make room for 'numBits' additional bits
			// as we store X-bit blocks, we can add max X bit's at once
			if (  unlikely(numBits >= writeableBits) ) {

				// all available bits will now be filled

				// get the bits to add
				uint32_t toAppend = writeableBits;
				BSO_TYPE tmp = value >> (numBits - toAppend);

				// shift the old bits up to make room for the new ones
				// and append only the requested region
				pending = (pending << toAppend) | tmp;

				// after this step, we still have to append this many bits
				numBits -= toAppend;

				// increment buffer, modulo for bit-pos
				bitPos = 0;
				*head = BSO_SWAP(pending);
				++head;

			} else {

				// shift the old bits up to make room for the new ones
				// and append only the requested region
				pending = (pending << numBits) | value;

				// we use this many bits in our 8-bit variable
				bitPos += numBits;

				// done
				return;

			}

		}


	}

	/** flush pending bits */
	void close() {
		flush();
	}


private:

	/** hidden copy ctor */
	BitstreamOut(const BitstreamOut&);

	/** flush currently pending bits */
	void flush() {

		// move latest bits upwards
		*head = BSO_SWAP(pending << (BSO_TYPE_BITS - bitPos));
		++head;

		// reset bit pointer for writing
		bitPos = 0;

	}



	/** the buffer to use */
	const uint8_t* const buffer;

	/** moving pointer for writing to buffer */
	BSO_TYPE* head;

	/** the element currently beeing processed */
	BSO_TYPE pending;

	/** current bit-position within pending */
	uint32_t bitPos;

};


#endif /* BITSTREAMOUT_H_ */
