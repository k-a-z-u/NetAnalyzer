#ifdef WITH_QUICKLZ

#ifndef QUICKLZ_COMPRESS_H_
#define QUICKLZ_COMPRESS_H_

#include "Compressor.h"

#include "../lib/QuickLZ/quicklz.h"

#include <vector>
#include <cstdint>

class QuickLZCompress : public Compressor {

public:

	QuickLZCompress();
	virtual ~QuickLZCompress();

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;
	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

private:

	/** is the stream initialized? */
	bool isOpen = false;

	/** internal state of the compression algorithm */
	qlz_state_compress* compressState = nullptr;

	/** uncompress into this buffer */
	std::vector<char> outBuffer;

};

#endif /* QUICKLZ_COMPRESS_H_ */

#endif // WITH_QUICKLZ
