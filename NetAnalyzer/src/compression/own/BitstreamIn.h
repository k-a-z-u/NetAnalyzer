/*
 * BitstreamIn.h
 *
 *  Created on: Apr 30, 2013
 *      Author: kazu
 */

#ifndef BITSTREAMIN_H_
#define BITSTREAMIN_H_

#include "opt.h"

#define BSI_TYPE			uint8_t
#define BSI_TYPE_BITS		(sizeof(BSI_TYPE) * 8)
//#define BSI_SWAP			__builtin_bswap64
#define BSI_SWAP
#define BSI_BIT_MASK		(1UL << (BSI_TYPE_BITS-1))

//#define BSI_SET_PENDING()	pending = BSI_SWAP(*tail);
#define BSI_SET_PENDING()

/**
 * represents a Bitstream that can be wrapped around an uint8_t*
 * in order to read bits from it.
 *
 * NOTES:
 * seems like using bytes here (instead of uint32_t and SWAP) in conjunction
 * with the branch-prediction-hint is faster..
 *
 */
class BitstreamIn {


public:

	/** ctor. empty! */
	BitstreamIn() :
		buffer(0), bufSize(0), bitPos(0), pending(0) {

		tail = (BSI_TYPE*) buffer;
		head = tail;

		BSI_SET_PENDING()

	}

	/** ctor. wrapped around the given data array */
	BitstreamIn(uint8_t* __restrict__ data, uint32_t size) :
		buffer(data), bufSize(size), bitPos(0), pending(0) {

		tail = (BSI_TYPE*) buffer;
		head = (BSI_TYPE*) ((uint8_t*) tail + size);

		BSI_SET_PENDING();

	}

	/** dtor */
	~BitstreamIn() {;}


	/**
	 * reset this Bitstream
	 * this will simply reposition the read pointer and read caches
	 */
	void reset() {
		bitPos = 0;
		tail = (BSI_TYPE*) buffer;
		head = (BSI_TYPE*) ((uint8_t*) tail + bufSize);
		BSI_SET_PENDING();
	}


	/** get number of bits available for reading */
	uint32_t getNumReadableBits() const {
		return (((uint8_t*) head - (uint8_t*) tail) << 3) - bitPos;
	}

	/** returns the number of fully available words */
	uint32_t getNumReadableWords() const {
		return ((uint8_t*) head - (uint8_t*) tail)  / sizeof(BSI_TYPE);
	}

	/** wrap around the given data array */
	void wrap(uint8_t* __restrict__ data, uint32_t size) {
		buffer = data;
		bufSize = size;
		reset();
	}


	void prefetch() {
		__builtin_prefetch(tail, 0, 1);
	}


	inline BSI_TYPE getWord() {
		return BSI_SWAP( *(tail++) );
	}

	/** get next bit from the stream */
	inline bool getBit() {


		bool out = ((*tail) << bitPos) & (1UL << (BSI_TYPE_BITS-1));
		//bool out = (pending << bitPos) & (1UL << (BSI_TYPE_BITS-1));

		if ( unlikely(++bitPos == BSI_TYPE_BITS) ) {
			//if (++bitPos == BSI_TYPE_BITS) {
			bitPos = 0;
			++tail;
			BSI_SET_PENDING();
		}

		return out;

	}

private:

	/** hidden copy ctor */
	BitstreamIn(const BitstreamIn&);


	/** the buffer to use */
	const uint8_t* __restrict__ buffer;

	/** the size of the buffer */
	uint32_t bufSize;

	/** current bit-position */
	uint32_t bitPos;

	/** moving pointer within buffer */
	BSI_TYPE* tail;

	/** fixed head (after end of buffer) for faster size calculation */
	BSI_TYPE* head;

	/** the element currently being processed */
	BSI_TYPE pending;

	//uint8_t pending[32];

};


#endif /* BITSTREAMIN_H_ */
