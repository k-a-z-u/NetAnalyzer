/*
 * DataBuffer.h
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "DataBufferData.h"
#include "../Debug.h"
//#include <iosfwd>

#define DBG_BUFFER			"BUFFER"


class DataBuffer {

public:

	/** ctor */
	DataBuffer() : bufSize(0), bufUsed(0), bufData(nullptr) {
		;
	}

	/** dtor */
	~DataBuffer() {
		if (bufData != nullptr) {free(bufData);}
	}

	/** append one byte to the buffer */
	inline void append(uint8_t byte) {

		// is the buffer big enough?
		ensureSize(bufUsed + 1);

		// append new data
		bufData[bufUsed] = byte;
		++bufUsed;

		debug(DBG_BUFFER, DBG_LVL_INFO, "appended 1 byte to the buffer. bytes available: " << bufUsed);

	}

	/**
	 * append the given bytes WITHOUT checking whether
	 * the buffer provides enough space for them!
	 *
	 * !! USE with caution !!
	 */
	inline void appendUnsafe(const uint8_t* data, uint32_t length) {

		// append new data
		memcpy(bufData + bufUsed, data, length);
		bufUsed += length;

		debug(DBG_BUFFER, DBG_LVL_INFO, "appended " << length << " bytes to the buffer. bytes available: " << bufUsed);

	}

	/** append the given data to the buffer */
	inline void append(const uint8_t* data, uint32_t length) {

		// is the buffer big enough?
		ensureSize(bufUsed + length);

		// append new data
		memcpy(bufData + bufUsed, data, length);
		bufUsed += length;

		debug(DBG_BUFFER, DBG_LVL_INFO, "appended " << length << " bytes to the buffer. bytes available: " << bufUsed);

	}

	/** append the given data to the buffer at the given offset */
	inline void appendAt(const uint8_t* data, uint32_t length, uint32_t offset) {

		// is the buffer bigh enough?
		ensureSize(offset + length);

		// append new data
		memcpy(bufData + offset, data, length);
		bufUsed += length;

		debug(DBG_BUFFER, DBG_LVL_INFO, "appended " << length << " bytes to the buffer @ offset " << offset);

	}


	/** get the index of the first occurance of "search" or -1 if not found */
	inline int32_t indexOf(const char* search) const {

		// get the length of the search-string
		uint32_t searchLen = strlen(search);

		// check buffers size (for-loop uses unsigned int and won't capture this problem)
		if (bufUsed < searchLen) {return -1;}

		// search for first occurrence
		for (uint32_t i = 0; i <= bufUsed - searchLen; ++i) {

			// first char matches, check following chars
			if (bufData[i] == search[0]) {

				// match the following chars
				bool found = true;
				for (uint32_t j = 1; j < searchLen; ++j) {
					if (bufData[i+j] != search[j]) {found = false; break;}
				}

				// matched?
				if (found) {return i;}

			}

		}

		// not found
		return -1;

	}

	/** get as const char* */
	inline const char* c_str() {
		return (const char*) bufData;
	}

	/** get the current content of buffer and clear it */
	inline DataBufferData get() {
		return DataBufferData(bufData, bufUsed, *this);
	}

	/**
	 * get the first numBytes bytes from the buffer and remove them from it.
	 * if numBytes > buffer's content only the available content will be returned
	 */
	inline DataBufferData get(uint32_t numBytes) {
		if (numBytes > bufUsed) {numBytes = bufUsed;}
		return DataBufferData(bufData, numBytes, *this);
	}

	/** clear the buffer */
	inline void clear() {
		bufUsed = 0;
	}

	/** dump the current content of the buffer */
	//void dump(std::ostream& target);

	/** dump content's in binary format */
	//void dumpBin(std::ostream& target);

	/** get the number of currently used bytes */
	inline uint32_t bytesUsed() const {
		return bufUsed;
	}

	/** get the empty space of the buffer, available for writing */
	inline uint32_t bytesUnused() const {
		return bufSize - bufUsed;
	}

	/** returns true if this buffer is empty */
	inline bool empty() const {
		return bufUsed == 0;
	}

	/**
	 * resize the DataBuffer to the given size.
	 * If growing, data will be retained.
	 * If shrinking, trailing bytes will be removed.
	 */
	inline void resize(uint32_t newSize) {

		// resize the buffer
		bufData = (uint8_t*) realloc(bufData, newSize);

		// sanity check (out of memory)
		assertNotNull(bufData, "out of memory error within DataBuffer");

		// set the new size
		bufSize = newSize;

		debug(DBG_BUFFER, DBG_LVL_INFO, "resized buffer to hold at least " << newSize << " bytes");

	}

protected:

	/**
	 * the DTOR of DataBufferData will remove
	 * the data from the beginning of the buffer
	 */

	friend struct DataBufferData;

	/** remove the provided data from the buffer */
	void remove(const DataBufferData& buf) {

		// skip if buffer is empty
		if (buf.length == 0) {return;}

		// if we remove everything from the buffer, simple reset the length-value (faster!)
		if (buf.length == bufUsed) {
			clear();
		} else {
			if (buf.length > bufUsed) {error(DBG_BUFFER, "remove: out of bounds. numBytes > bufUsed.");}
			memmove(bufData, bufData + buf.length, bufUsed - buf.length);
			bufUsed -= buf.length;
		}

		debug(DBG_BUFFER, DBG_LVL_INFO, "removed " << buf.length << " bytes from the buffer. bytes available: " << bufUsed);

	}


private:

	/** ensure that the buffer is able to store at least 'size' bytes */
	inline void ensureSize(uint32_t neededSize) {

		// do we really need more space in the buffer?
		if ( __builtin_expect(neededSize < bufSize, 1) ) {return;}

		// we will allocate twice the needed size
		uint32_t newSize = neededSize << 1;

		// perform resize
		resize(newSize);

	}

	/** the total size of the buffer */
	uint32_t bufSize;

	/** the number of currently used bytes */
	uint32_t bufUsed;

	/** store data here */
	uint8_t* bufData;

};


#endif /* DATABUFFER_H_ */
