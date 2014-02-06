#ifdef WITH_LZO

#ifndef LZO_DECOMPRESS_H_
#define LZO_DECOMPRESS_H_

#include "Decompressor.h"

#include <vector>
#include <cstdint>

class LZODecompress : public Decompressor {

public:

	LZODecompress();
	virtual ~LZODecompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** decompress into this buffer */
	uint8_t* decompBuffer;

};

#endif /* LZO_DECOMPRESS_H_ */

#endif // WITH_LZO
