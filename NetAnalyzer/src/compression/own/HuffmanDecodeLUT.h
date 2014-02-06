/*
 * HuffmanDecodeLUT.h
 *
 *  Created on: May 1, 2013
 *      Author: kazu
 */

#ifndef HUFFMANDECODELUT_H_
#define HUFFMANDECODELUT_H_

/** unused entry within decoding LUT */
#define HUFFMAN_EMPTY	0xFFFF
#include <cstring>

struct HuffmanDecodeEntry {

	/** the decoded symbol */
	uint16_t symbol;

	/**
	 * the number of bits used for this index.
	 * needed to distinguish between 0001 and 1 which
	 * would both have the index '1'
	 */
	uint8_t numBits;

	/** ctor */
	HuffmanDecodeEntry() : symbol(HUFFMAN_EMPTY), level(0) {;}

};


/**
 * "LUT" used for decoding
 * unused bit-combinations will be marked as HUFFMAN_EMPTY
 */
struct HuffmanDecodeLUT {

	/** array to look-up entries by their bit-combination */
	HuffmanDecodeEntry entry[0xFFFFFF];

	/** ctor */
	HuffmanDecodeLUT() : entry() {;}

};


#endif /* HUFFMANDECODELUT_H_ */
