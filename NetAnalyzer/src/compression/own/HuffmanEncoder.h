/*
 * HuffmanEncoder.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef HUFFMANENCODER_H_
#define HUFFMANENCODER_H_

#include "HuffmanEncodeLUT.h"


// forward declarations
struct HuffmanTreeNode;
class BitstreamOut;

/**
 * encode data using a fixed huffman tree
 */
class HuffmanEncoder {

public:

	/** ctor */
	HuffmanEncoder(const HuffmanEncodeLUT& lut);

	/** dtor */
	virtual ~HuffmanEncoder();


	/** provides access to the underlying LUT */
	const HuffmanEncodeLUT& getLUT() const;

	/** encode the given bytes */
	void append(const uint8_t* data, uint32_t length, BitstreamOut& stream);

	/** finish encoding. this will append EOF and flush the bitstream */
	void close(BitstreamOut& stream);

	/** reset everything for a new compression */
	void reset(BitstreamOut& stream);


private:

	/** hidden copy ctor */
	HuffmanEncoder(const HuffmanEncoder&);

	/** the look-up-table to use for encoding */
	const HuffmanEncodeLUT lut;

};

#endif /* HUFFMANENCODER_H_ */
