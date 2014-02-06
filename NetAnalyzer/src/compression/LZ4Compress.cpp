#ifdef WITH_LZ4

#include "LZ4Compress.h"
#include "Data.h"
#include "../Debug.h"

#include "../lib/LZ4/lz4.h"

#include <cstring>

#define DBG_LZ4		"LZ4"

/** The maximum number of bytes to be compressed in one API call. */
static const size_t IN_BUF_SIZE = 64 * 1024;

/** The maximum buffer for each compressed chunk. Must be large enough to
	deal with uncompressable data where compression increases its size. */
static const size_t OUT_BUF_SIZE = LZ4_compressBound(IN_BUF_SIZE);

LZ4Compress::LZ4Compress() : isOpen(false), outBuffer(OUT_BUF_SIZE) {
	reset();

	// set the default name for this compressor
	setName("LZ4Compress");
}

LZ4Compress::~LZ4Compress() {
	reset();
}

Compressor* LZ4Compress::create() const {
	auto* ptr = new LZ4Compress();
	ptr->setName(getName());
	return ptr;
}

bool LZ4Compress::open() {

	if (isOpen) {
		debug(DBG_LZ4, DBG_LVL_WARN, "Trying to open to an already open compression stream.");
		return false;
	}
	reset();

	isOpen = true;
	return true;

}

//bool LZ4Compress::append(const Data& toCompress, size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZ4, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
//		return false;
//	}
//	compressedSize = 0;
//
//	if (!toCompress.length) {
//		return true;
//	}
//
//	// Use the class's internal buffer to compress the given data and return
//	// its compressed size.
//	size_t bytesLeft = toCompress.length;
//
//	while (bytesLeft) {
//		const size_t bytesToCompr = std::min(bytesLeft, IN_BUF_SIZE);
//		const char* srcData = (char*)toCompress.data + toCompress.length - bytesLeft;
//
//		const size_t outSize = LZ4_compress(srcData, outBuffer.data(), bytesToCompr);
//		if (outSize == 0) {
//			// 0 indicates an error
//			error(DBG_LZ4, "LZ4_compress() failed!");
//		}
//
//		compressedSize += outSize;
//		bytesLeft -= bytesToCompr;
//	}
//
//	return true;
//
//}
//
//bool LZ4Compress::close(size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZ4, DBG_LVL_WARN, "Trying to close a closed compression stream.");
//		return false;
//	}
//
//	compressedSize = 0;
//
//	reset();
//	return true;
//
//}

bool LZ4Compress::append(const Data& toCompress, std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_LZ4, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
		return false;
	}

	if (toCompress.length == 0) {
		return true;
	}

	const size_t oldResultSize = result.size();
	/* Make it large enough to deal with uncompressable data. */
	result.resize(oldResultSize + LZ4_compressBound(toCompress.length));

	char* destData = result.data() + oldResultSize;
	const size_t outSize = LZ4_compress((const char*)toCompress.data, destData, toCompress.length);
	if (outSize == 0) {
		// 0 indicates an error
		error(DBG_LZ4, "LZ4_compress() failed!");
	}

	result.resize(oldResultSize + outSize);
	return true;

}

bool LZ4Compress::close(std::vector<char>& result) {

	(void) result;

	if (!isOpen) {
		debug(DBG_LZ4, DBG_LVL_WARN, "Trying to close a closed compression stream.");
		return false;
	}

	reset();
	return true;

}

void LZ4Compress::reset() {

	isOpen = false;

}


#endif // WITH_LZ4
