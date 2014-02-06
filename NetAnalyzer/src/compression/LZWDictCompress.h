/*
 * LZWDictCompress.h
 *
 *  Created on: May 23, 2013
 *      Author: kazu
 */

#ifndef LZWDICTCOMPRESS_H_
#define LZWDICTCOMPRESS_H_

#include "lzwDict/LZWDictStream.h"
#include "lzwDict/LZWDictEncoder.h"
#include "Compressor.h"

// forward declarations
class LZWDict;

class LZWDictCompress : public Compressor {
public:

	LZWDictCompress(const LZWDict& dict);

	virtual ~LZWDictCompress();

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;
	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

private:

	/** the dictionary to use for compression */
	const LZWDict& dict;

	/** the stream to compress to */
	LZWDictStream stream;

	/** the encoder to use */
	LZWDictEncoder enc;

};

#endif /* LZWDICTCOMPRESS_H_ */
