/*
 * LZWDictDecoder.h
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#ifndef LZWDICTDECODER_H_
#define LZWDICTDECODER_H_

#include <cstdint>

#include "../../buffer/DataBuffer.h"
#include "LZWDict.h"

class LZWDictDecoder {
public:

	LZWDictDecoder(const LZWDict& dict);

	~LZWDictDecoder();

	/** decompress the given data */
	void append(const uint8_t* data, uint32_t length, DataBuffer& db);

private:

	/** the dictionary to use for decompression */
	const LZWDict* dict;

};

#endif /* LZWDICTDECODER_H_ */
