#ifndef LZMA_DECOMPRESS_H_
#define LZMA_DECOMPRESS_H_

#ifdef WITH_LZMA

#include "Decompressor.h"

#include <lzma.h>

#include <vector>
#include <cstdint>

class LZMADecompress : public Decompressor {

public:

	LZMADecompress();
	virtual ~LZMADecompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** decompress into this buffer */
	uint8_t* decompBuffer;

	/** the maximum size for each uncompressed chunk */
	//static const size_t BUF_SIZE = 64 * 1024;

	/** is the stream initialized ? */
	bool isOpen = false;

	/** the stream to use for decompression */
	lzma_stream stream = LZMA_STREAM_INIT;

};

#endif // WITH_LZMA

#endif /* LZMA_DECOMPRESS_H_ */
