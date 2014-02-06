/*
 * Decompressor.h
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifndef DECOMPRESSOR_H_
#define DECOMPRESSOR_H_

#include <vector>
#include <cstdint>

struct Data;

/**
 * interface for all decompressors
 */
class Decompressor {

public:

	/** dtor */
	virtual ~Decompressor() {;}

	/** append data for decompresson and receive some decompressed bytes */
	//	virtual bool append(const Data& toDecompress, Data& ret) = 0;


	/**
	 * Create a new Decompressor object with the same settings as the original.
	 * This is not called 'clone()' because it is not intended to clone the
	 * whole object's internal status (buffer, etc.) but to preserve the
	 * original object's decompression settings.
	 */
	virtual Decompressor* create() const = 0;

	/** Decompress the original data and save the decompressed data in result. */
	virtual bool append(const Data& toDecompress, std::vector<uint8_t>& result) = 0;

	/** reset the decompressor to its initial state */
	virtual void reset() = 0;
};

#endif /* DECOMPRESSOR_H_ */
