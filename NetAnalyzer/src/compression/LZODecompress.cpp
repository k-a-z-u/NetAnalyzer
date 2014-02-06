#ifdef WITH_LZO

#include "LZODecompress.h"
#include "Data.h"
#include "../Debug.h"

#include <lzo/lzo1x.h>

#include <cstdlib>
#include <cstring>

#define DBG_LZO		"LZO_DEC"

/** The maximum buffer for each compressed chunk. */
static const size_t OUT_BUF_SIZE = 512 * 1024;

LZODecompress::LZODecompress() {

	if (lzo_init() != LZO_E_OK) {
		error(DBG_LZO, "lzo_init() failed.");
	}

	decompBuffer = (uint8_t*) malloc(OUT_BUF_SIZE);

}

LZODecompress::~LZODecompress() {
	delete(decompBuffer);
}

Decompressor* LZODecompress::create() const {
	auto* ptr = new LZODecompress();
	return ptr;
}

bool LZODecompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}


	// HACK: the size of the decompressed data chunk may not be larger than OUT_BUF_SIZE!
	// FIXME: allow decompressed output of any size!


	// make room the the uncompressed chunk of data
	//const size_t oldResultSize = result.size();
	//result.resize(oldResultSize + OUT_BUF_SIZE);

	const unsigned char* srcData = toDecompress.data;
	const lzo_uint srcSize = toDecompress.length;
	//unsigned char* destData = result.data() + oldResultSize;
	unsigned char* destData = decompBuffer;
	lzo_uint outBytes = 0;

	const auto rc = lzo1x_decompress(srcData, srcSize, destData, &outBytes, nullptr);
	if (rc != LZO_E_OK) {
		// TODO: check for all those other obscure LZO_E_* return values.
		error(DBG_LZO, "lzo1x_decompress() failed with error code: " << rc);
	}

	// this is part of the HACK mentioned above. if the output buffer was filled
	// we probably have more compressed data waiting to be decompressed.
	if (outBytes == OUT_BUF_SIZE) {
		error(DBG_LZO, "the data to be decompressed is too large.");
	}

	// shrink the result buffer to the right size
	//result.resize(oldResultSize + outBytes);
	result.resize(outBytes);

	// copy the bytes
	//memcpy(&result[0]+oldResultSize, decompBuffer, outBytes);
	memcpy(&result[0], decompBuffer, outBytes);


	return true;

}

void LZODecompress::reset() {

}

#endif // WITH_LZO
