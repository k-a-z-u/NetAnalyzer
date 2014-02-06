#ifdef WITH_LZMA

#include "LZMACompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cassert>

#define DBG_LZMA		"LZMA"

/** the maximum buffer for each uncompressed chunk */
static const size_t BUF_SIZE = 64 * 1024;

LZMACompress::LZMACompress() : isOpen(false), inpDataBytes(0), buffer(BUF_SIZE), stream(LZMA_STREAM_INIT) {
	reset();

	// set the default name for this compressor
	setName("LZMACompress");
}

LZMACompress::LZMACompress(const uint32_t lvl) : LZMACompress() {
	setLevel(lvl);
}

LZMACompress::~LZMACompress() {
	reset();
}

Compressor* LZMACompress::create() const {
	auto* ptr = new LZMACompress();
	ptr->setName(getName());
	ptr->setLevel(level);
	return ptr;
}

void LZMACompress::setLevel(const uint32_t lvl) {
	if ((lvl < 1) || (lvl > 9)) {
		error(DBG_LZMA, "Invalid compression level! Must be a number between 1 and 9.");
	}
	level = lvl;
}

bool LZMACompress::open() {

	if (isOpen) {
		debug(DBG_LZMA, DBG_LVL_WARN, "Trying to open to an already open compression stream.");
		return false;
	}
	reset();

	// init with the given preset and set the integrity check to crc64
	if (lzma_easy_encoder(&stream, level, LZMA_CHECK_CRC64) != LZMA_OK) {
		debug(DBG_LZMA, DBG_LVL_ERR, "Opening stream failed.");
		return false;
	}

	isOpen = true;
	return true;

}

//bool LZMACompress::append(const Data& toCompress, size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZMA, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
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
//	stream.next_in = toCompress.data;
//	stream.avail_in = toCompress.length;
//
//	// we will consume all given input in BUF_SIZE-sized chunks
//	while (stream.avail_in) {
//		// where to compress to
//		stream.next_out = (unsigned char*)buffer.data();
//		stream.avail_out = BUF_SIZE;
//
//		// compress
//		const auto rc = lzma_code(&stream, LZMA_RUN);
//		if (rc != LZMA_OK) {
//			error(DBG_LZMA, "compress() failed: " << rc);
//		}
//
//		// the compressed size of the last chunk
//		compressedSize += (BUF_SIZE - stream.avail_out);
//	}
//	return true;
//
//}
//
//bool LZMACompress::close(size_t& compressedSize) {
//
//	if (!isOpen) {
//		debug(DBG_LZMA, DBG_LVL_WARN, "Trying to close a closed compression stream.");
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
//	// extract all remaining data from the compression stream
//	while (true) {
//		// where to compress to
//		stream.next_out = (unsigned char*)buffer.data();
//		stream.avail_out = BUF_SIZE;
//
//		const auto rc = lzma_code(&stream, LZMA_FINISH);
//
//		if (rc == LZMA_STREAM_END) {
//			// we got all data from the stream, everything is peachy
//			compressedSize += (BUF_SIZE - stream.avail_out);
//			break;
//		} else if (rc == LZMA_OK) {
//			// there is still data left inside the stream
//			// in this case stream.avail_out should usually be 0
//			compressedSize += (BUF_SIZE - stream.avail_out);
//		} else {
//			error(DBG_LZMA, "close() failed: " << rc);
//		}
//	}
//
//	reset();
//	return true;
//
//}

bool LZMACompress::append(const Data& toCompress, std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_LZMA, DBG_LVL_WARN, "Trying to append to a closed compression stream.");
		return false;
	}

	if (toCompress.length == 0) {
		return true;
	}

	// track number of input data bytes we tried to compress.
	inpDataBytes += toCompress.length;

	// what to compress
	stream.next_in = toCompress.data;
	stream.avail_in = toCompress.length;

	// we will consume all given input in BUF_SIZE-sized chunks
	while (stream.avail_in) {
		// make room for the uncompressed data chunk
		const size_t oldResultSize = result.size();
		result.resize(oldResultSize + BUF_SIZE);

		// where to compress to
		stream.next_out = (unsigned char*)result.data() + oldResultSize;
		stream.avail_out = BUF_SIZE;

		const auto rc = lzma_code(&stream, LZMA_RUN);
		if (rc != LZMA_OK) {
			error(DBG_LZMA, "lzma_code() failed with error code " << rc);
		}

		// shrink the result buffer to the actual output size
		result.resize(result.size() - stream.avail_out);
	}
	return true;

}

bool LZMACompress::close(std::vector<char>& result) {

	if (!isOpen) {
		debug(DBG_LZMA, DBG_LVL_WARN, "Trying to close a closed compression stream.");
		return false;
	}

	// finish compression (only if we have compressed something!)
	if (inpDataBytes == 0) {
		reset();
		return true;
	}

	// extract all remaining data from the compression stream
	while (true) {
		// make room for the uncompressed data chunk
		const size_t oldResultSize = result.size();
		result.resize(oldResultSize + BUF_SIZE);

		// where to compress to
		stream.next_out = (unsigned char*)result.data() + oldResultSize;
		stream.avail_out = BUF_SIZE;

		const auto rc = lzma_code(&stream, LZMA_FINISH);

		if (rc == LZMA_STREAM_END) {
			// we got all data from the stream, everything is peachy
			result.resize(result.size() - stream.avail_out);
			break;
		} else if (rc == LZMA_OK) {
			// there is still data left inside the stream
			// in this case stream.avail_out should usually be 0
			result.resize(result.size() - stream.avail_out);
		} else {
			error(DBG_LZMA, "close() failed: " << rc);
		}
	}

	reset();
	return true;

}

void LZMACompress::reset() {

	if (isOpen) {
		lzma_end(&stream);
	}
	stream = LZMA_STREAM_INIT;

	isOpen = false;
	inpDataBytes = 0;

}

#endif // WITH_LZMA
