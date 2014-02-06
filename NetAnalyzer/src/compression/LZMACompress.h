#ifndef LZMA_COMPRESS_H_
#define LZMA_COMPRESS_H_

#ifdef WITH_LZMA

#include "Compressor.h"

#include <lzma.h>

#include <vector>
#include <cstdint>

class LZMACompress : public Compressor {

public:

	LZMACompress();
	LZMACompress(uint32_t lvl);
	virtual ~LZMACompress();

	/** preset is the compression level, can be numbers from 0-9. */
	void setLevel(uint32_t lvl);

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;
	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

private:

	/** is the stream initialized? */
	bool isOpen;

	/** count the number of data-bytes passed into the compressor */
	int inpDataBytes;

	/** uncompress into this buffer */
	std::vector<char> buffer;

	/** the stream to use for compression */
	lzma_stream stream;

	/** compression level, can be numbers from 0-9. */
	uint32_t level = 0;

};

#endif // WITH_LZMA

#endif /* LZMA_COMPRESS_H_ */
