/*
 * GZip.h
 *
 * provides GZIP uncompression capabilities using ZLIB
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifdef WITH_ZLIB

#ifndef GZIP_DECOMPRESS_H_
#define GZIP_DECOMPRESS_H_

#include "Decompressor.h"

#include <zlib.h>

#include <vector>
#include <cstdint>

class GZipDecompress : public Decompressor {

public:

	/** ctor */
	GZipDecompress();

	/** dtor */
	virtual ~GZipDecompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** decompress into this buffer */
	uint8_t* decompBuffer;

	/** initialize the stream (once) */
	void initUncompress();

	/** the maximum buffer for each uncompressed chunk */
	//static const size_t BUF_SIZE = 64 * 1024;

	/** the stream to use for decompression */
	z_stream stream;

	/** is the stream initialized ? */
	bool isOpen = false;

};

#endif /* GZIP_DECOMPRESS_H_ */

#endif /* WITH_ZLIB */
