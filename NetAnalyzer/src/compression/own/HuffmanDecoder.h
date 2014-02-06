/*
 * HuffmanDecoder.h
 *
 *  Created on: Apr 30, 2013
 *      Author: kazu
 */

#ifndef HUFFMANDECODER_H_
#define HUFFMANDECODER_H_

#include <cstdint>
#include <vector>

//#include "HuffmanTree.h"
//#include "HuffmanDecodeLUT.h"

class HuffmanTree;
class HuffmanTreeNode;
class BitstreamIn;

/**
 * decode data using a fixed huffman tree
 */
class HuffmanDecoder {

public:

	/** ctor */
	HuffmanDecoder(const HuffmanTree* tree);

	/** dtor */
	virtual ~HuffmanDecoder();

	/** decode all available bits in the given bitstream to the destionation buffer */
	bool append(BitstreamIn& stream, std::vector<uint8_t>& dst);

private:

	/** the tree to use for decoding */
	const HuffmanTreeNode* root;

	/** the last node where decoder stopped at */
	const HuffmanTreeNode* lastNode;

	//HuffmanDecodeLUT lut;

	//uint32_t curBits = 0;

};

#endif /* HUFFMANDECODER_H_ */
