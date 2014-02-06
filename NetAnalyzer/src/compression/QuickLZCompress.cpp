#ifdef WITH_QUICKLZ

#include "QuickLZCompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cstring>

#define DBG_QUICKLZ		"QuickLZ"

/** the maximum buffer for each uncompressed chunk */
static const size_t IN_BUF_SIZE = 64 * 1024;

/** The maximum buffer for each compressed chunk. Must be large enough to
	deal with uncompressable data where compression increases its size.
	Value obtained from quicklz documentation.*/
static const size_t OUT_BUF_SIZE = IN_BUF_SIZE + 400;

QuickLZCompress::QuickLZCompress() : outBuffer(OUT_BUF_SIZE) {

	compressState = new qlz_state_compress;

	// set zero out compressState members
	reset();

	// set the default name for this compressor
	setName("QuickLZCompress");
}

QuickLZCompress::~QuickLZCompress() {
	reset();

	delete compressState;
	compressState = nullptr;
}

Compressor* QuickLZCompress::create() const {
	auto* ptr = new QuickLZCompress();
	ptr->setName(getName());
	return ptr;
}

bool QuickLZCompress::open() {

	if (isOpen) {
		debug(DBG_QUICKLZ, DBG_LVL_WARN, "Trying to open to an already open compression stream.");
		return false;
	}
	reset();

	isOpen = true;
	return true;

}

//bool QuickLZCompress::append(const Data& toCompress, size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_QUICKLZ, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
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
//		const size_t comprSize = qlz_compress(srcData, outBuffer.data(),
//											  bytesToCompr, compressState);
//
//		compressedSize += comprSize;
//		bytesLeft -= bytesToCompr;
//	}
//
//	return true;
//
//}
//
//bool QuickLZCompress::close(size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_QUICKLZ, DBG_LVL_WARN, "Trying to close a closed compression stream.");
//		return false;
//	}
//
//	compressedSize = 0;
//
//	reset();
//	return true;
//
//}

bool QuickLZCompress::append(const Data& toCompress, std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_QUICKLZ, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
		return false;
	}

	if (toCompress.length == 0) {
		return true;
	}

	const size_t oldResultSize = result.size();
	/* Make it large enough to deal with uncompressable data. See quicklz documentation. */
	result.resize(oldResultSize + toCompress.length + 400);

	char* destData = result.data() + oldResultSize;
	const size_t comprSize = qlz_compress(toCompress.data, destData,
										  toCompress.length, compressState);

	result.resize(oldResultSize + comprSize);
	return true;

}

bool QuickLZCompress::close(std::vector<char>& result) {

	(void) result;

	if (!isOpen) {
		debug(DBG_QUICKLZ, DBG_LVL_WARN, "Trying to close a closed compression stream.");
		return false;
	}

	reset();
	return true;

}

void QuickLZCompress::reset() {

	memset(compressState, 0, sizeof(qlz_state_compress));
	isOpen = false;

}

#endif // WITH_QUICKLZ
