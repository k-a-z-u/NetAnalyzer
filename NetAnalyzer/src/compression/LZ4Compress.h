#ifdef WITH_LZ4

#ifndef LZ4_COMPRESS_H_
#define LZ4_COMPRESS_H_

#include "Compressor.h"

#include <vector>
#include <cstdint>

class LZ4Compress : public Compressor {

public:

	LZ4Compress();
	virtual ~LZ4Compress();

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

	/** uncompress into this buffer */
	std::vector<char> outBuffer;

};

#endif /* LZ4_COMPRESS_H_ */


#endif // WITH_LZ4
