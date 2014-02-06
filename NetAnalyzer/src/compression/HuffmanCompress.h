/*
 * HuffmanCompress.h
 *
 *  Created on: Apr 29, 2013
 *      Author: kazu
 */

#ifndef HUFFMANCOMPRESS_H_
#define HUFFMANCOMPRESS_H_

#include "Compressor.h"
#include "own/HuffmanEncodeLUT.h"
#include "own/BitstreamOut.h"

class HuffmanEncoder;

#include <vector>
#include <cstdint>

class HuffmanCompress : public Compressor {

public:

	HuffmanCompress(const HuffmanEncodeLUT& lut);

	virtual ~HuffmanCompress();

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;

	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

private:

	/** the encoding LUT */
	HuffmanEncodeLUT lut;

	/** the encoder to use */
	HuffmanEncoder* enc;

	/** the bitstream to write to */
	BitstreamOut* stream;

	/** the buffer for the bitstream */
	uint8_t* buffer;

};

#endif /* HUFFMANCOMPRESS_H_ */
