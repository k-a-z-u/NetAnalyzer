/*
 * DictEncoder.cpp
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#include "DictEncoder.h"
#include "DictFlex.h"
#include <iostream>
#include "../../buffer/DataBuffer.h"
#include "opt.h"

DictEncoder::DictEncoder(const DictFlex* dict) : dict(dict) {
	;
}

DictEncoder::~DictEncoder() {
	;
}



void DictEncoder::append(__restrict__ const uint8_t* data, register uint32_t length, DictEncStream& stream) {

	// proceed until end-of-data
	while( likely(length>3) ) {

		uint32_t code;
		uint32_t consumed = 0;

		dict->getCode(data, length, code, consumed);

		// dictionary hit??
		if ( unlikely(consumed!=0) ) {

			uint32_t val = code << 4 | consumed;
			stream.head[0] = (val >> 8 );
			stream.head[1] = (val);

			stream.head += 2;
			length -= consumed;
			data += consumed;

		} else {

			stream.head[0] = *data;
			++stream.head;

			--length;
			++data;

		}

	}

	// append last chars
	while (length > 0) {
		stream.head[0] = *data;
		++stream.head;
		--length;
		++data;
	}

}
