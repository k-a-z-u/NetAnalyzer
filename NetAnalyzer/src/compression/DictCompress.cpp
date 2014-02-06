/*
 * DictCompress.cpp
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#include "DictCompress.h"


#include "dict/DictEncoder.h"
#include "Data.h"

DictCompress::DictCompress(const DictFlex& dict) : enc(nullptr), dict(dict), stream(1024*1024) {
	enc = new DictEncoder(&dict);

	// set the default name for this compressor
	setName("DictCompress");
}

DictCompress::~DictCompress() {
	if (enc != nullptr)		{delete enc; enc = nullptr;}
}

Compressor* DictCompress::create() const {
	auto* ptr = new DictCompress(dict);
	ptr->setName(getName());
	return ptr;
}

bool DictCompress::open() {
	stream.reset();
	return true;
}

//bool DictCompress::append(const Data& toCompress, size_t& compressedSize) {
//	stream.reset();
//	enc->append(toCompress.data, toCompress.length, stream);
//	compressedSize = stream.getNumBytes();
//	return true;
//}
//
//bool DictCompress::close(size_t& compressedSize) {
//	stream.reset();
//	compressedSize = 0;
//	return true;
//}



bool DictCompress::append(const Data& toCompress, std::vector<char>& result) {
	stream.reset();
	enc->append(toCompress.data, toCompress.length, stream);
	return true;
}

bool DictCompress::close(std::vector<char>& result) {
	stream.reset();
	return true;
}

void DictCompress::reset() {
	stream.reset();
}
