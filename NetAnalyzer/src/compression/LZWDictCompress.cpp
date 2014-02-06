/*
 * LZWDictCompress.cpp
 *
 *  Created on: May 23, 2013
 *      Author: kazu
 */

#include "LZWDictCompress.h"

#include "lzwDict/LZWDict.h"
#include "Data.h"

LZWDictCompress::LZWDictCompress(const LZWDict& dict) : dict(dict), stream(1024*1024), enc(LZWDictEncoder(dict)) {

	// set the default name for this compressor
	setName("LZWDict");

}

LZWDictCompress::~LZWDictCompress() {
	;
}

Compressor* LZWDictCompress::create() const {
	auto* ptr = new LZWDictCompress(dict);
	ptr->setName(getName());
	return ptr;
}

bool LZWDictCompress::open() {
	stream.clear();
	return true;
}

//bool LZWDictCompress::append(const Data& toCompress, size_t& compressedSize) {
//	stream.clear();
//	enc.append(toCompress.data, toCompress.length, stream);
//	compressedSize = stream.getNumUsed();
//	return true;
//}
//
//bool LZWDictCompress::close(size_t& compressedSize) {
//	stream.clear();
//	compressedSize = 0;
//	return true;
//}



bool LZWDictCompress::append(const Data& toCompress, std::vector<char>& result) {

	enc.append(toCompress.data, toCompress.length, stream);

	// resize the result-buffer and copy the data
	result.resize(stream.getNumUsed());
	memcpy(&result[0], stream.getData(), stream.getNumUsed());

	stream.clear();

	return true;

}

bool LZWDictCompress::close(std::vector<char>& result) {

	// write unflushed bytes
	enc.close(stream);

	// resize the result-buffer and copy the data
	result.resize(stream.getNumUsed());
	memcpy(&result[0], stream.getData(), stream.getNumUsed());

	// TODO transfer bytes from stream to result!
	return true;

}

void LZWDictCompress::reset() {
	stream.clear();
}
