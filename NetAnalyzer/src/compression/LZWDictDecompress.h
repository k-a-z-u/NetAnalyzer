/*
 * LZWDictDecompress.h
 *
 *  Created on: Jun 11, 2013
 *      Author: kazu
 */

#ifndef LZWDICTDECOMPRESS_H_
#define LZWDICTDECOMPRESS_H_

#include "lzwDict/LZWDictDecoder.h"
#include "../buffer/DataBuffer.h"
#include "Decompressor.h"

// forward declarations
class LZWDict;



class LZWDictDecompress : public Decompressor {
public:

	LZWDictDecompress(const LZWDict& dict);
	virtual ~LZWDictDecompress();

	bool append(const Data& toDecompress, std::vector<uint8_t>& result) override;

	void reset() override;

	Decompressor* create() const override;

private:

	/** the dictionary to use for decompression */
	const LZWDict& dict;

	/** the decoder used for decompression */
	LZWDictDecoder dec;

	/** buffer for decompression */
	DataBuffer db;

};

#endif /* LZWDICTDECOMPRESS_H_ */
