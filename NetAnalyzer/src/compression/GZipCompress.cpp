/*
 * GZipCompress.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifdef WITH_ZLIB

#include "GZipCompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cassert>
#include <cstring>

#define DBG_GZIP		"GZIP"

/** the maximum buffer for each uncompressed chunk */
static const size_t BUF_SIZE = 64 * 1024;

GZipCompress::GZipCompress() : isOpen(false), inpDataBytes(0), buffer(BUF_SIZE), stream(), dict() {
	reset();

	// set the default name for this compressor
	setName("GZipCompress");
}

GZipCompress::~GZipCompress() {
	reset();
}

void GZipCompress::setCompressionLevel(const int lvl) {
	level = lvl;
}

void GZipCompress::setStrategy(const int strat) {
	strategy = strat;
}

Compressor* GZipCompress::create() const {
	auto* ptr = new GZipCompress();
	ptr->setName(getName());
	ptr->setCompressionLevel(level);
	ptr->setStrategy(strategy);
	ptr->setDictionary(dict);
	return ptr;
}

/** set the dictionary to use for compression */
void GZipCompress::setDictionary(const std::string& dict) {
	this->dict = dict;
}

bool GZipCompress::open() {

	if (isOpen) {
		debug(DBG_GZIP, DBG_LVL_WARN, "Trying to open to an already open compression stream.");
		return false;
	}
	reset();

	// initialize ZLIB for GZIP format / header
	if (deflateInit2(&stream, level, Z_DEFLATED, MAX_WBITS, MAX_MEM_LEVEL, strategy) != Z_OK) {
		debug(DBG_GZIP, DBG_LVL_ERR, "Opening stream failed.");
		return false;
	}

	// if dictionary is pre-defined -> use it
	if (!dict.empty()) {
		int ret = deflateSetDictionary(&stream, (const Bytef*) dict.c_str(), dict.length());
		if (ret != Z_OK) {error(DBG_GZIP, "error while setting dictionary!");}
	}

	isOpen = true;
	return true;

}

//bool GZipCompress::append(const Data& toCompress, size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_GZIP, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
//		return false;
//	}
//	compressedSize = 0;
//
//	if (!toCompress.length) {
//		return true;
//	}
//
//	// track number of input data bytes we tried to compress.
//	inpDataBytes += toCompress.length;
//
//	// what to compress
//	stream.next_in = (unsigned char*)toCompress.data;
//	stream.avail_in = toCompress.length;
//
//	do {
//		// where to compress to
//		stream.next_out = (unsigned char*)buffer.data();
//		stream.avail_out = BUF_SIZE;
//
//		// deflate() can not fail here. it either returns Z_OK or it returns
//		// Z_BUF_ERROR which means that stream.avail_in was 0 (which is ok also).
//		deflate(&stream, Z_NO_FLUSH);
//
//		// the compressed size of the last chunk
//		compressedSize += (BUF_SIZE - stream.avail_out);
//	} while (stream.avail_out == 0);
//
//	assert(stream.avail_in == 0);
//	return true;
//
//}
//
//bool GZipCompress::close(size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_GZIP, DBG_LVL_WARN, "Trying to close a closed compression stream.");
//		return false;
//	}
//	compressedSize = 0;
//
//	// finish compression (only if we have compressed something!)
//	if (inpDataBytes == 0) {
//		reset();
//		return true;
//	}
//
//	do {
//		// where to compress to
//		stream.next_out = (unsigned char*)buffer.data();
//		stream.avail_out = BUF_SIZE;
//
//		deflate(&stream, Z_FINISH);
//
//		// the compressed size of the last chunk
//		compressedSize += (BUF_SIZE - stream.avail_out);
//	} while (stream.avail_out == 0);
//
//	assert(stream.avail_in == 0);
//	reset();
//	return true;
//
//}

bool GZipCompress::append(const Data& toCompress, std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_GZIP, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
		return false;
	}

	if (toCompress.length == 0) {
		return true;
	}

	// track number of input data bytes we tried to compress.
	inpDataBytes += toCompress.length;

	const size_t oldResultSize = result.size();
	// TODO: make the expected output data size depend on the input data size.
	// be sure to remember the amount of data already put into the stream but
	// not yet received.
	result.resize(oldResultSize + BUF_SIZE);

	// what to compress
	stream.next_in = (unsigned char*)toCompress.data;
	stream.avail_in = toCompress.length;

	// where to compress to
	stream.next_out = (unsigned char*)result.data() + oldResultSize;
	stream.avail_out = result.size() - oldResultSize;

	deflate(&stream, Z_NO_FLUSH);

	assert(stream.avail_in == 0);
	result.resize(result.size() - stream.avail_out);
	return true;

}

bool GZipCompress::close(std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_GZIP, DBG_LVL_WARN, "Trying to close a closed compression stream.");
		return false;
	}

	// finish compression (only if we have compressed something!)
	if (inpDataBytes == 0) {
		reset();
		return true;
	}

	do {
		const size_t oldResultSize = result.size();
		result.resize(oldResultSize + BUF_SIZE); // should be a good baseline

		// where to compress to
		stream.next_out = (unsigned char*)result.data() + oldResultSize;
		stream.avail_out = BUF_SIZE;

		deflate(&stream, Z_FINISH);

		result.resize(oldResultSize + BUF_SIZE - stream.avail_out);
	} while (stream.avail_out == 0);

	assert(stream.avail_in == 0);
	reset();
	return true;

}

void GZipCompress::reset() {

	if (isOpen) {
		deflateEnd(&stream);
	}
	memset(&stream, 0, sizeof(stream));

	isOpen = false;
	inpDataBytes = 0;

}

#endif /* WITH_ZLIB */
