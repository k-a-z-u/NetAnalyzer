/*
 * HuffmanEncoder.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#include <cstring>

#include "HuffmanEncoder.h"
#include "HuffmanTree.h"
#include "HuffmanTreeNode.h"
#include "HuffmanEncodeLUT.h"
#include "BitstreamOut.h"

#include "../../Debug.h"
#include "../../Helper.h"

HuffmanEncoder::HuffmanEncoder(const HuffmanEncodeLUT& lut) : lut(lut) {

}

HuffmanEncoder::~HuffmanEncoder() {

}



void HuffmanEncoder::append(const uint8_t* data, register uint32_t length, BitstreamOut& stream) {
	stream.prefetch();
	while (length != 0) {
		stream.append(lut.byte[*data].bitCode, lut.byte[*data].numBits);
		++data;
		--length;
	}
}

void HuffmanEncoder::close(BitstreamOut& stream) {

	// append EOF marker and flush stream
	stream.append(lut.byte[HUFFMAN_EOF].bitCode, lut.byte[HUFFMAN_EOF].numBits);
	stream.close();

}

void HuffmanEncoder::reset(BitstreamOut& stream) {
	stream.reset();
}

const HuffmanEncodeLUT& HuffmanEncoder::getLUT() const {
	return lut;
}

