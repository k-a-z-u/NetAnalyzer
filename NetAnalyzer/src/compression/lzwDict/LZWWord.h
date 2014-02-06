/*
 * LZWWord.h
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#ifndef LZWWORD_H_
#define LZWWORD_H_

#include <cstring>
#include <string>

#define LZW_WORD_LEN	16

struct LZWWord {


	/** bytes for this word */
	union {
		size_t		sized	[LZW_WORD_LEN / sizeof(size_t)];
		uint64_t	max		[LZW_WORD_LEN / sizeof(uint64_t)];
		uint32_t	ints	[LZW_WORD_LEN / sizeof(uint32_t)];
		uint16_t	shorts	[LZW_WORD_LEN / sizeof(uint16_t)];
		uint8_t		bytes	[LZW_WORD_LEN / sizeof(uint8_t)];
	} data;

	/** number of used bytes */
	uint32_t length;

	/** the index of this word to use for compression */
	uint32_t idx;

	/** pointer to next word */
	LZWWord* next;


	/** ctor */
	inline LZWWord() {
		clear();
	}

	/** copy ctor */
	inline LZWWord(const LZWWord& other) : length(other.length), idx(other.idx), next(nullptr) {
		memcpy(data.bytes, other.data.bytes, LZW_WORD_LEN);
	}

	/** ctor word from string */
	inline LZWWord(const std::string& str) {
		clear();
		length = str.length();
		memcpy(data.bytes, (uint8_t*)str.c_str(), length);
	}



	/** add one byte to this word */
	inline void add(const uint8_t byte) {
		data.bytes[length++] = byte;
	}

	/** set this word to empty */
	inline void clear() {
		memset(this, 0, sizeof(LZWWord));
		//length = 0;
		//memset(data.bytes, 0, LZW_WORD_LEN);
		// FIXME add for 32 bit code!
		//data.sized[0] = 0;
		//data.sized[1] = 0;
	}

	/** is this word empty? */
	inline bool empty() const {
		return length == 0;
	}


	/** get hash of this word */
	inline uint32_t getHash() const {
		uint32_t ret = length;
		ret += (data.ints[0]);
		ret += (data.ints[1]);
		ret += (data.ints[2]);
		ret += (data.ints[3]);
		return ret;
	}

	/** compare this word with another one */
	inline bool equals(const LZWWord& other) const {
		//return (length == other.length && memcmp(data.bytes, other.data.bytes, length) == 0);

		// best for 64 bit
		return	length == other.length &&
				data.sized[0] == other.data.sized[0] &&
				data.sized[1] == other.data.sized[1];
		// TODO: add for 32 bit

	}

};

/** used for decoding */
struct LZWDecWord {

	/** bytes for this word */
	union {
		size_t		sized	[LZW_WORD_LEN / sizeof(size_t)];
		uint64_t	max		[LZW_WORD_LEN / sizeof(uint64_t)];
		uint32_t	ints	[LZW_WORD_LEN / sizeof(uint32_t)];
		uint16_t	shorts	[LZW_WORD_LEN / sizeof(uint16_t)];
		uint8_t		bytes	[LZW_WORD_LEN / sizeof(uint8_t)];
	} data;

	uint32_t length;

	LZWDecWord() : length(0) {;}

	/** is this word empty? */
	inline bool empty() const {
		return length == 0;
	}

	/** add one byte to this word */
	inline void add(const uint8_t byte) {
		data.bytes[length++] = byte;
	}

	/** set this word to empty */
	inline void clear() {
		length = 0;
	}

};


#endif /* LZWWORD_H_ */
