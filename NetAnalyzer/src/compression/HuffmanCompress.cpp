/*
 * HuffmanCompress.cpp
 *
 *  Created on: Apr 29, 2013
 *      Author: kazu
 */

#include "HuffmanCompress.h"
#include "own/HuffmanEncoder.h"
#include "Data.h"

HuffmanCompress::HuffmanCompress(const HuffmanEncodeLUT& lut) :
	lut(lut), enc(nullptr) {

	// set the default name for this compressor
	setName("HuffmanCompress");

	enc = new HuffmanEncoder(lut);
	buffer = (uint8_t*) malloc(1024*1024);
	stream = new BitstreamOut(buffer);

}

HuffmanCompress::~HuffmanCompress() {
	if (buffer != nullptr)	{free (buffer); buffer = nullptr;}
	if (stream != nullptr)	{delete stream; stream = nullptr;}
	if (enc != nullptr)		{delete enc; enc = nullptr;}
}

Compressor* HuffmanCompress::create() const {
	auto* ptr = new HuffmanCompress(lut);
	ptr->setName(getName());
	return ptr;
}

bool HuffmanCompress::open() {

//	// cleanup
//	if (enc != nullptr) {
//		delete enc;
//		enc = nullptr;
//	}
//
//	// init the encoder
//	enc = new HuffmanEncoder(lut);
	enc->reset(*stream);

	return true;

}

//bool HuffmanCompress::append(const Data& toCompress, size_t& compressedSize) {
//	enc->append(toCompress.data, toCompress.length, *stream);
//	compressedSize = stream->getBytesAvailable();
//	stream->rewind();
//	return true;
//}
//
//bool HuffmanCompress::close(size_t& compressedSize) {
//	stream->close();
//	compressedSize = stream->getBytesAvailable();
//	stream->reset();
//	return true;
//}



bool HuffmanCompress::append(const Data& toCompress, std::vector<char>& result) {
	// TODO write bytes from stream to result!!
	enc->append(toCompress.data, toCompress.length, *stream);
	result.resize(stream->getBytesAvailable());
	stream->rewind();
	return true;
}

bool HuffmanCompress::close(std::vector<char>& result) {
	// TODO write bytes from stream to result!!
	stream->close();
	result.resize(stream->getBytesAvailable());
	stream->reset();
	return true;
}

void HuffmanCompress::reset() {
	enc->reset(*stream);
}
