#ifdef WITH_LZ4

#ifndef LZ4_DECOMPRESS_H_
#define LZ4_DECOMPRESS_H_

#include "Decompressor.h"

#include <vector>
#include <cstdint>

class LZ4Decompress : public Decompressor {

public:

	LZ4Decompress();
	virtual ~LZ4Decompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** decompress into this buffer */
	uint8_t* decompBuffer;

};

#endif /* LZ4_DECOMPRESS_H_ */

#endif // WITH_LZ4
