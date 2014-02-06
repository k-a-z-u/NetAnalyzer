/*
 * LZWDictDecompress.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: kazu
 */

#include "LZWDictDecompress.h"

#include "lzwDict/LZWDict.h"
#include "Data.h"

LZWDictDecompress::LZWDictDecompress(const LZWDict& dict) : dict(dict), dec(LZWDictDecoder(dict)) {

	// we use a fixed-size DataBuffer for performance reasons
	db.resize(64*1024);

}

LZWDictDecompress::~LZWDictDecompress() {
	// TODO Auto-generated destructor stub
}

Decompressor* LZWDictDecompress::create() const {
	auto* ptr = new LZWDictDecompress(dict);
	return ptr;
}

bool LZWDictDecompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}

	// TODO add data transfer from db to result!
	dec.append(toDecompress.data, toDecompress.length, db);
	result.resize(db.bytesUsed());
	db.clear();

	return true;

}

void LZWDictDecompress::reset() {

	db.clear();

}
