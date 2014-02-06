/*
 * LZWDictEncoder.h
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#ifndef LZWDICTENCODER_H_
#define LZWDICTENCODER_H_

#include <cstdint>
#include "LZWWord.h"


// forward declarations
class LZWDictStream;
class LZWDict;
class LZWTreeNode;

/**
 * LZW-based dictionary encoding
 */
class LZWDictEncoder {

public:

	LZWDictEncoder(const LZWDict& dict);

	~LZWDictEncoder();

	/** compress the provided data into the given LZWDictStream */
	void append(const uint8_t* data, const uint32_t length, LZWDictStream& dst);

	/** close the encoder. flushes pending data and (appends EOF) */
	void close(LZWDictStream& dst);

private:

	/** the dictionary to use for compression */
	const LZWDict& dict;

	/** the current node for traversing the dictionary */
	const LZWTreeNode* curNode;

	/** the root-node for the dictionary */
	const LZWTreeNode* rootNode;

	/** the currently pending word */
	LZWWord pending;

};

#endif /* LZWDICTENCODER_H_ */
