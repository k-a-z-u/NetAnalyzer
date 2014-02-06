/*
 * Bitstream.h
 *
 * provides bit-stream operations
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef BITSTREAM_H_
#define BITSTREAM_H_

#include <iosfwd>
#include <cstdint>
#include <vector>
#include <cstdlib>

#include <iostream>
#include "../../Debug.h"

#include "../../buffer/DataBuffer.h"

#define BS_STORE_TYPE		size_t
#define BS_MAX_BITS			(sizeof(BS_STORE_TYPE) * 8)
#define BS_GET_MASK			(1UL << (BS_MAX_BITS-1))

// use fixed-size bitstream (do not use grow())
// faster! but: buffer MUST be cleared or SIGSEGV! ;)
#define BS_FIXED_SIZE		1024*1024


class Bitstream {

public:

	/** ctor */
	Bitstream() : bitPos(0), bitReadPos(BS_MAX_BITS-1) {

#ifdef BS_FIXED_SIZE
		bufSize = BS_FIXED_SIZE;
#else
		bufSize = sizeof(BS_STORE_TYPE) * 32;
#endif

		buffer = (uint8_t*) malloc(bufSize);

		bufPtr = (BS_STORE_TYPE*) buffer;
		bufReadPtr = (BS_STORE_TYPE*) buffer;

	}

	/** dtor */
	virtual ~Bitstream() {
		if (buffer != nullptr) {free(buffer);}
	}

	/**
	 * reset this bitstream
	 * this will simply reposition the read- and write-pointers
	 * (WON'T free() any memory!)
	 */
	void reset() {
		bufPtr = (BS_STORE_TYPE*) buffer;
		bufReadPtr = (BS_STORE_TYPE*) buffer;
		bitPos = 0;
		bitReadPos = BS_MAX_BITS - 1;
	}

	/**
	 * get one whole byte from the bitstream
	 * this function will treat the buffer as a byte array.
	 * can be used to transfer the buffers's content.
	 * WARNING: do NOT mix with getBit()!!
	 */
	uint8_t getByte() {
		uint8_t ret = *((uint8_t*) bufReadPtr);
		bufReadPtr = (BS_STORE_TYPE*) (((uint8_t*) bufReadPtr) + 1);
		return ret;
	}


	/**
	 * append one byte to the bitstream
	 * this will ignore current bit position and might overwrite some bits!
	 * can be used to set buffers content like a byte-array
	 * WARNING: do NOT mix with append(value, numBits)!
	 */
	void append(uint8_t byte) {
		*((uint8_t*) bufPtr) = byte;
		bufPtr = (BS_STORE_TYPE*) (((uint8_t*) bufPtr) + 1);
	}

	/** append the lowest 'numBits' bits of 'value' */
	void append(uint32_t value, uint32_t numBits) {

#ifndef BS_FIXED_SIZE
		growIfNeeded();
#endif

		// proceed until all bits have been flushed to the buffer
		while(true) {

			// we need to make room for 'numBits' additional bits
			// as we store X-bit blocks, we can add max X bit's at once
			if (numBits > BS_MAX_BITS - bitPos) {

				// get the bits to add
				uint32_t toAppend = BS_MAX_BITS - bitPos;
				BS_STORE_TYPE tmp = value >> (numBits - toAppend);

				// we still have to append this many bits
				numBits -= toAppend;

				// shift the old bits up to make room for the new ones
				*bufPtr <<= toAppend;

				// append only the requested region
				*bufPtr |= tmp;

				// we use this many bits in our 8-bit variable
				bitPos += toAppend;

				//				if (bitPos == BS_MAX_BITS) {
				//					bitPos = 0;
				//					++bufPtr;
				//				}
				bufPtr += bitPos / BS_MAX_BITS;
				bitPos &= BS_MAX_BITS-1;

			} else {

				// shift the old bits up to make room for the new ones
				*bufPtr <<= numBits;

				// append only the requested region
				*bufPtr |= value;

				// we use this many bits in our 8-bit variable
				bitPos += numBits;


				//				if (bitPos == BS_MAX_BITS) {
				//					bitPos = 0;
				//					++bufPtr;
				//				}
				bufPtr += bitPos == BS_MAX_BITS;
				bitPos &= BS_MAX_BITS-1;

				return;

			}

		}


	}

	/** flush pending bits */
	void close() {
		flush();
	}

	/** get number of bits available for reading */
	uint32_t getNumReadableBits() {

		uint32_t bits = ((uint8_t*) bufPtr - (uint8_t*) bufReadPtr) * 8;
		bits += (bitPos);
		bits -= (BS_MAX_BITS - bitReadPos - 1);

		return bits;

	}

	/** get the number of full bytes available for reading (full byte = all bits used) */
	uint32_t getNumReadableBytes() const {
		uint32_t cnt = (uint8_t*) bufPtr - (uint8_t*) bufReadPtr;
		return (cnt / sizeof(BS_STORE_TYPE)) * sizeof(BS_STORE_TYPE);
	}

	/** write all complete bytes into the given buffer */
	void getCompleteBytes(std::vector<uint8_t>& dstBuf) {

		// write all available bytes into the vector
		const uint32_t complete = getNumCompleteBytes();
		for (uint32_t i = 0; i < complete / sizeof(BS_STORE_TYPE); ++i) {
			BS_STORE_TYPE val = ((BS_STORE_TYPE*) buffer)[i];

			// FIXME
			// currently only working with 64 bits
			dstBuf.push_back(val >> 56);
			dstBuf.push_back(val >> 48);
			dstBuf.push_back(val >> 40);
			dstBuf.push_back(val >> 32);
			dstBuf.push_back(val >> 24);
			dstBuf.push_back(val >> 16);
			dstBuf.push_back(val >> 8);
			dstBuf.push_back(val >> 0);

		}

		// move the buffer
		memmove(&buffer[0], &buffer[complete], bufSize - complete);

		// reset write pointer
		bufPtr = (BS_STORE_TYPE*) buffer;

	}

	/** get the number of available bytes (all bits used) */
	uint32_t getNumCompleteBytes() const {
		return (uint8_t*) bufPtr - (uint8_t*) buffer;
	}

	/** get one bit from the stream */
	bool getBit() {

		// get next bit
		bool out;
		out = (*bufReadPtr & (1UL << (bitReadPos)));

		// update bit-position
		if (bitReadPos == 0) {
			bitReadPos = BS_MAX_BITS-1;
			++bufReadPtr;
		} else {
			--bitReadPos;
		}

		return out;

	}

private:

	/** hidden copy ctor */
	Bitstream(const Bitstream&);

	/** flush current pending bits */
	void flush() {

		// move latest bits upwards
		*bufPtr <<= BS_MAX_BITS - bitPos;
		++bufPtr;

		// reset bit pointer for writing
		bitPos = 0;

	}

#ifndef BS_FIXED_SIZE

	/** resize the buffer it's nearly out of free-space */
	void growIfNeeded() {

		// get number of used bytes
		uint32_t usedBytes = (uint8_t*) bufPtr - (uint8_t*) buffer;

		// do we need to grow the buffer?
		if (usedBytes >= bufSize / 2) {

			// store the old buffer pointer
			uint8_t* oldBufPtr = buffer;

			// resize the buffer to double size
			bufSize *= 2;
			buffer = (uint8_t*) realloc(buffer, bufSize);

			// check
			if (buffer == 0) {error("BITSTREAM", "out of memory error! requested size was: " << bufSize);}

			//std::cout << "growing to: " << bufSize << std::endl;

			// check if the buffers memory address changed
			int64_t dist = buffer - oldBufPtr;

			if (dist != 0) {

				//std::cout << "old pos: " << (size_t) oldBufPtr << std::endl;
				//std::cout << "new pos: " << (size_t) buffer << std::endl;
				//std::cout << "dist: " << dist << std::endl;

				//std::cout << "2 old pos: " << (size_t) bufPtr << std::endl;
				bufPtr = (BS_STORE_TYPE*) (((uint8_t*) bufPtr) + dist);
				bufReadPtr = (BS_STORE_TYPE*) (((uint8_t*) bufReadPtr) + dist);

				//std::cout << "2 new pos: " << (size_t) bufPtr << std::endl;
			}

		}
	}

#endif

	/** the buffer to use */
	uint8_t* buffer;

	/** current bit-position (writing) */
	uint32_t bitPos;

	/** current bit-position (reading) */
	uint32_t bitReadPos;

	/** the size of the buffer */
	uint32_t bufSize;

	/** moving pointer within buffer */
	BS_STORE_TYPE* bufPtr;

	/** moving pointer within buffer */
	BS_STORE_TYPE* bufReadPtr;

};

#endif /* BITSTREAM_H_ */
