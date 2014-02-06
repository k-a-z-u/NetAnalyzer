/*
 * GZipUncompress.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifdef WITH_ZLIB

#include "GZipDecompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cstring>

#define DBG_GZIP		"GZIP"

static const size_t OUT_BUF_SIZE = 512 * 1024;


GZipDecompress::GZipDecompress() {
	decompBuffer = (uint8_t*) malloc(OUT_BUF_SIZE);
}

GZipDecompress::~GZipDecompress() {
	if (isOpen) {
		inflateEnd(&stream);
	}
	delete(decompBuffer);
}

Decompressor* GZipDecompress::create() const {
	auto* ptr = new GZipDecompress();
	return ptr;
}

bool GZipDecompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}

	// init (once)
	if (!isOpen) {
		initUncompress();
	}

	// size that the output buffer was passed into this function
	// the buffer will be reset to its initial state if an error occures
	//const auto originalResultSize = result.size();

	// what to decompress
	stream.next_in = (uint8_t*)toDecompress.data;
	stream.avail_in = toDecompress.length;

	// we will consume all given input in BUF_SIZE-sized chunks
	while (stream.avail_in) {

		// make room for the uncompressed data chunk
		const size_t oldResultSize = result.size();
		//result.resize(oldResultSize + OUT_BUF_SIZE);

		// where to decompress to
		//stream.next_out = (unsigned char*)result.data() + oldResultSize;
		stream.next_out = decompBuffer;
		stream.avail_out = OUT_BUF_SIZE;

		// perform decompression
		const auto ret = inflate(&stream, Z_SYNC_FLUSH);
		if ((ret != Z_OK) && (ret != Z_STREAM_END)) {
			//result.resize(originalResultSize); // reset result buffer
			debug(DBG_GZIP, DBG_LVL_ERR, "inflate failed: " << stream.msg);
			return false;
		}

		// shrink the result buffer to the actual output size
		size_t outBytes = OUT_BUF_SIZE - stream.avail_out;
		result.resize(oldResultSize + outBytes);

		// copy the bytes
		memcpy(&result[0]+oldResultSize, decompBuffer, outBytes);
		//memcpy(&result[0], decompBuffer, outBytes);


	}
	return true;

}

void GZipDecompress::reset() {

	if (isOpen) {
		inflateEnd(&stream);
	}
	std::memset(&stream, 0, sizeof(stream));

	isOpen = false;

}

void GZipDecompress::initUncompress() {

	stream.total_out = 0;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;

	// initialize ZLIB for GZIP format / header
	if (inflateInit2(&stream, (15+32)) != Z_OK) {error(DBG_GZIP, "failed!");}
	isOpen = true;

}

#endif /* WITH_ZLIB */
