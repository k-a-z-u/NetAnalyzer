#ifdef WITH_LZ4

#include "LZ4Decompress.h"
#include "Data.h"
#include "../Debug.h"

#include "../lib/LZ4/lz4.h"

#include <cstdlib>
#include <cstring>

#define DBG_LZ4		"LZ4_DEC"

/** The maximum buffer for each compressed chunk. */
static const size_t OUT_BUF_SIZE = 512 * 1024;

LZ4Decompress::LZ4Decompress() {
	decompBuffer = (uint8_t*) malloc(OUT_BUF_SIZE);
}

LZ4Decompress::~LZ4Decompress() {
	delete(decompBuffer);
}

Decompressor* LZ4Decompress::create() const {
	auto* ptr = new LZ4Decompress();
	return ptr;
}

bool LZ4Decompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}


	// HACK: the size of the decompressed data chunk may not be larger than OUT_BUF_SIZE!
	// FIXME: allow decompressed output of any size!


	// make room the the uncompressed chunk of data
	//const size_t oldResultSize = result.size();
	//result.resize(oldResultSize + OUT_BUF_SIZE);

	const char* srcData = (char*)toDecompress.data;
	char* destData = (char*) decompBuffer;
	//char* destData = (char*)result.data() + oldResultSize;
	const int inputSize = toDecompress.length;

	const int outBytes = LZ4_decompress_safe(srcData, destData, inputSize, OUT_BUF_SIZE);
	if (outBytes < 0) {
		// negative return value indicates an error
		error(DBG_LZ4, "LZ4_decompress_safe() failed!");
	}

	// this is part of the HACK mentioned above. if the output buffer was filled
	// we probably have more compressed data waiting to be decompressed.
	if (outBytes == OUT_BUF_SIZE) {
		error(DBG_LZ4, "the data to be decompressed is too large.");
	}

	// shrink the result buffer to the right size
	//result.resize(oldResultSize + outBytes);
	result.resize(outBytes);

	// copy the bytes
	//memcpy(&result[0]+oldResultSize, decompBuffer, outBytes);
	memcpy(&result[0], decompBuffer, outBytes);

	return true;

}

void LZ4Decompress::reset() {

}


#endif // WITH_LZ4
