/*
 * GZip.h
 *
 * provides GZIP compression capabilities using ZLIB
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifdef WITH_ZLIB

#ifndef GZIP_COMPRESS_H_
#define GZIP_COMPRESS_H_

#include "Compressor.h"

#include <zlib.h>

#include <vector>
#include <cstdint>
#include <string>

class GZipCompress : public Compressor {

public:

	GZipCompress();
	virtual ~GZipCompress();

	void setCompressionLevel(int lvl);
	void setStrategy(int strat);

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;
	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

	/** set the dictionary to use for compression */
	void setDictionary(const std::string& dict);

private:

	/** is the stream initialized? */
	bool isOpen;

	/** count the number of data-bytes passed into the compressor */
	int inpDataBytes;

	/** uncompress into this buffer */
	std::vector<char> buffer;

	/** the stream to use for compression */
	z_stream stream;

	int level = Z_DEFAULT_COMPRESSION;
	int strategy = Z_DEFAULT_STRATEGY;

	/** the dictionary to use (if any) */
	std::string dict;

};

#endif /* GZIP_COMPRESS_H_ */

#endif /* WITH_ZLIB */
