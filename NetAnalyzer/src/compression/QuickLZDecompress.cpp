#ifdef WITH_QUICKLZ

#include "QuickLZDecompress.h"
#include "Data.h"
#include "../Debug.h"

#include <cstdlib>
#include <cstring>

#define DBG_QUICKLZ		"QLZ_DEC"

QuickLZDecompress::QuickLZDecompress() {
	reset();
}

QuickLZDecompress::~QuickLZDecompress() {
	delete decompressState;
	decompressState = nullptr;
}

Decompressor* QuickLZDecompress::create() const {
	auto* ptr = new QuickLZDecompress();
	return ptr;
}

bool QuickLZDecompress::append(const Data& toDecompress, std::vector<uint8_t>& result) {

	if (toDecompress.length == 0) {
		return true;
	}

	// pointer and size of the data to be decompressed
	const char* srcData = (char*)toDecompress.data;

	// make room the the uncompressed chunk of data
	const size_t sizeDecompr = qlz_size_decompressed(srcData);
	const size_t oldResultSize = result.size();
	result.resize(oldResultSize + sizeDecompr);

	// pointer to the buffer the data will be decompressed into
	char* destData = (char*)result.data() + oldResultSize;

	// does not take any kind of srcSize argument because the information about
	// that should be encoded at the beginning of srcData. be sure to always
	// use the same settings for compression that will be used for decompression!
	const size_t retSizeDecompr = qlz_decompress(srcData, destData, decompressState);

	if (retSizeDecompr != sizeDecompr) {
		error(DBG_QUICKLZ, "the actual size of the decompressed data does not match the expected size");
	}

	return true;
}

void QuickLZDecompress::reset() {
	// delete the current state if it exists
	delete decompressState;

	// allocate a new state and initialize it
	decompressState = new qlz_state_decompress;
	std::memset(decompressState, 0, sizeof(qlz_state_decompress));
}

#endif // WITH_QUICKLZ
