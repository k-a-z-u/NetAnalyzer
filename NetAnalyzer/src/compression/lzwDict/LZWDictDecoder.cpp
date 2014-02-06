/*
 * LZWDictDecoder.cpp
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#include "LZWDictDecoder.h"
#include "../../Debug.h"
#include "opt.h"

#define LZW_FAST_MODE

//idee:
//statt ein string-pointer array für das decode zu verwenden vlt ein
//array was idx nach offset+length mapped. und alle decode string
//dann in einen großen malloc bereich packen aus dem mit offset+length
//die werte geholt werden? evtl is das schneller

LZWDictDecoder::LZWDictDecoder(const LZWDict& dict) : dict(&dict) {
	// TODO Auto-generated constructor stub

}

LZWDictDecoder::~LZWDictDecoder() {
	// TODO Auto-generated destructor stub
}

#include <iostream>
void LZWDictDecoder::append(const uint8_t* data, register uint32_t length, DataBuffer& db) {

#ifdef LZW_FAST_MODE

	// fast-mode requires length to be a multiple of 2
	assertTrue(length % 2 == 0, "length must be a multiple of 2");

	uint16_t* ptr = (uint16_t*) data;

	// decode all bytes
	while( likely(length) ) {

		// read 2 bytes to get the dict's index
		//uint32_t curIdx = (data[0] << 8) | (data[1] << 0);
		uint32_t curIdx = *ptr;
		// next 2 bytes
		++ptr;
		//data += 2;
		length -= 2;

		// get word behind this index
		const LZWDecWord& word = dict->getWord(curIdx);

		// sanity check
		assertTrue(!word.empty(), "decoded word was empty! index was: " << (curIdx&0xFFFF) );

		// append to output
		// WARNING: output buffer's size is not validated for performance reasons!
		db.appendUnsafe(word.data.bytes, word.length);

	}

#else

	assertTrue(length % 2 == 0, "length must be a multiple of 2");

	// store the current index
	static uint32_t curIdx = 0;
	static uint32_t cnt = 0;

	// decode all bytes
	for (register uint32_t i = 0; i < length; ++i) {

		curIdx <<= 8;
		curIdx |= data[i];
		++cnt;

		// 2 bytes read
		if (cnt == 2) {


			uint32_t curIdx = (data[i+0] << 8) | (data[i+1] << 0);

			// get word behind this index
			const LZWWord& word = dict->getWord(curIdx & 0xFFFF);

			// sanity check
			assertTrue(!word.empty(), "decoded word was empty! index was: " << (curIdx&0xFFFF) );

			// append to output
			db.append( (uint8_t*) word.data.bytes, word.length);

		}

	}

#endif

}
