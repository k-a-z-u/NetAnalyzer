#ifdef WITH_LZMA

#include "LZMADecompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cstring>
#include <cassert>

#define DBG_LZMA		"LZMA_DEC"

static const size_t OUT_BUF_SIZE = 512 * 1024;

LZMADecompress::LZMADecompress() {
	decompBuffer = (uint8_t*) malloc(OUT_BUF_SIZE);
}

LZMADecompress::~LZMADecompress() {
	if (isOpen) {
		lzma_end(&stream);
	}
	delete(decompBuffer);
}

Decompressor* LZMADecompress::create() const {
	auto* ptr = new LZMADecompress();
	return ptr;
}

bool LZMADecompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}

	if (!isOpen) {
		const auto rc = lzma_stream_decoder(&stream, UINT64_MAX, LZMA_TELL_NO_CHECK | LZMA_TELL_UNSUPPORTED_CHECK);
		if (rc != LZMA_OK) {
			error(DBG_LZMA, "decompressor initialization failed with error code " << rc);
		}
		isOpen = true;
	}

	// size that the output buffer was passed into this function
	// the buffer will be reset to its initial state if an error occures
	//const auto originalResultSize = result.size();

	// what to decompress
	stream.next_in = toDecompress.data;
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
		const auto rc = lzma_code(&stream, LZMA_RUN);
		if ((rc != LZMA_OK) && (rc != LZMA_STREAM_END)) {
			//result.resize(originalResultSize); // reset result buffer
			debug(DBG_LZMA, DBG_LVL_ERR, "lzma_code() failed with error code " << rc);
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

void LZMADecompress::reset() {

	if (isOpen) {
		lzma_end(&stream);
	}
	stream = LZMA_STREAM_INIT;

	isOpen = false;

}

#endif // WITH_LZMA
