#ifdef WITH_LZO

#ifndef LZO_COMPRESS_H_
#define LZO_COMPRESS_H_

#include "Compressor.h"

#include <vector>
#include <cstdint>

class LZOCompress : public Compressor {

public:

	LZOCompress();
	virtual ~LZOCompress();

	void setCompressionLevel(int lvl);

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
	std::vector<unsigned char> outBuffer;

	/** use this buffer while compressing */
	std::vector<unsigned char> workBuffer;

	int level;

};

#endif /* LZO_COMPRESS_H_ */

#endif // WITH_LZO
