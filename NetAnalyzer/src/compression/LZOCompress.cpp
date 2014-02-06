#ifdef WITH_LZO

#include "LZOCompress.h"
#include "Data.h"
#include "../Debug.h"

#include <lzo/lzo1x.h>

#include <cstring>

#define DBG_LZO		"LZO"

/** The maximum number of bytes to be compressed in one API call. */
static const size_t IN_BUF_SIZE = 64 * 1024;

/** The maximum buffer for each compressed chunk. Must be large enough to
	deal with uncompressable data where compression increases its size.
	Value obtained from lzo documentation (examples/simple.c).*/
static const size_t OUT_BUF_SIZE = IN_BUF_SIZE + (IN_BUF_SIZE / 16 + 64 + 3);

LZOCompress::LZOCompress()
	: isOpen(false)
	, outBuffer(OUT_BUF_SIZE)
	// always use a working buffer as large as the best compression level needs
	, workBuffer(std::max(LZO1X_1_MEM_COMPRESS, LZO1X_999_MEM_COMPRESS))
	, level(1) {
	reset();

	if (lzo_init() != LZO_E_OK) {
		error(DBG_LZO, "lzo_init() failed.");
	}

	// set the default name for this compressor
	setName("LZOCompress");
}

LZOCompress::~LZOCompress() {
	reset();
}

void LZOCompress::setCompressionLevel(const int lvl) {
	if ((lvl != 1) && (lvl != 9)) {
		error(DBG_LZO, "Invalid compression level! Only 1 and 9 are allowed.");
	}
	level = lvl;
}

Compressor* LZOCompress::create() const {
	auto* ptr = new LZOCompress();
	ptr->setName(getName());
	ptr->setCompressionLevel(level);
	return ptr;
}

bool LZOCompress::open() {

	if (isOpen) {
		debug(DBG_LZO, DBG_LVL_WARN, "Trying to open to an already open compression stream.");
		return false;
	}
	reset();

	isOpen = true;
	return true;

}

//bool LZOCompress::append(const Data& toCompress, size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZO, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
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
//		const unsigned char* srcData = toCompress.data + toCompress.length - bytesLeft;
//
//		size_t outSize = 0;
//
//		/* calls to compress should never fail according to the documentation */
//		if (level == 1) {
//			lzo1x_1_compress(srcData, bytesToCompr, outBuffer.data(), &outSize, workBuffer.data());
//		} else {
//			lzo1x_999_compress(srcData, bytesToCompr, outBuffer.data(), &outSize, workBuffer.data());
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
//bool LZOCompress::close(size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZO, DBG_LVL_WARN, "Trying to close a closed compression stream.");
//		return false;
//	}
//
//	compressedSize = 0;
//
//	reset();
//	return true;
//
//}

bool LZOCompress::append(const Data& toCompress, std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_LZO, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
		return false;
	}

	if (toCompress.length == 0) {
		return true;
	}

	const size_t oldResultSize = result.size();
	/* Make it large enough to deal with uncompressable data. See lzo documentation. */
	result.resize(oldResultSize + toCompress.length + (toCompress.length / 16 + 64 + 3));

	unsigned char* destData = (unsigned char*)result.data() + oldResultSize;
	size_t outSize = 0;

	if (level == 1) {
		lzo1x_1_compress(toCompress.data, toCompress.length, destData, &outSize, workBuffer.data());
	} else {
		lzo1x_999_compress(toCompress.data, toCompress.length, destData, &outSize, workBuffer.data());
	}

	result.resize(oldResultSize + outSize);
	return true;

}

bool LZOCompress::close(std::vector<char>& result) {

	(void) result;

	if (!isOpen) {
		debug(DBG_LZO, DBG_LVL_WARN, "Trying to close a closed compression stream.");
		return false;
	}

	reset();
	return true;

}

void LZOCompress::reset() {

	isOpen = false;

}

#endif // WITH_LZO
