#ifdef WITH_QUICKLZ

#ifndef QUICKLZ_DECOMPRESS_H_
#define QUICKLZ_DECOMPRESS_H_

#include "Decompressor.h"

#include "../lib/QuickLZ/quicklz.h"

#include <vector>
#include <cstdint>

class QuickLZDecompress : public Decompressor {

public:

	QuickLZDecompress();
	virtual ~QuickLZDecompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** internal state of the compression algorithm */
	qlz_state_decompress* decompressState = nullptr;

};

#endif /* QUICKLZ_DECOMPRESS_H_ */

#endif // WITH_QUICKLZ
