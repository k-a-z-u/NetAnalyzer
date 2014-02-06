/*
 * HuffmanEncodeLUT.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef HUFFMANENCODELUT_H_
#define HUFFMANENCODELUT_H_

#include <cstdint>


/**
 * one entry within the huffman-encoding-LUT
 */
struct HuffmanEncodeEntry {

	/** the bit-code for this entry */
	uint32_t bitCode;

	/** the number of bits of the code to use (max 255 bits) */
	uint32_t numBits;

	/** is this entry valid? (replacement for a null-pointer) */
	//bool isValid;

	/** ctor */
	HuffmanEncodeEntry() : bitCode(0), numBits(0) {;}

};


/**
 * lookup-table to encode one byte to a huffman code
 */
struct HuffmanEncodeLUT {

	/**
	 * the look-up array.
	 * contains huffman-code for every bit.
	 * byte[256] = END OF STREAM marker
	 */
	HuffmanEncodeEntry byte[256+1];

};


#endif /* HUFFMANENCODELUT_H_ */
