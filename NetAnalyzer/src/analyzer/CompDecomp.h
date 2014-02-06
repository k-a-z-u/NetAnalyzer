/*
 * CompDecomp.h
 *
 *  Created on: Jun 5, 2013
 *      Author: kazu
 */

#ifndef COMPDECOMP_H_
#define COMPDECOMP_H_

#include "../compression/Compressor.h"
#include "../compression/Decompressor.h"

/**
 * compressor and corresponding decompressor pair
 */
struct CompressorDecompressor {

	Compressor* comp = nullptr;

	Decompressor* decomp = nullptr;

	/** ctor */
	CompressorDecompressor(Compressor* c, Decompressor* d) : comp(c), decomp(d) {;}

	/** dtor */
	~CompressorDecompressor() {
		if (comp != nullptr)	{delete comp; comp = nullptr;}
		if (decomp != nullptr)	{delete decomp; decomp = nullptr;}
	}

	/** copy ctor */
	CompressorDecompressor(const CompressorDecompressor& old) {

		// copy
		this->comp = old.comp;
		this->decomp = old.decomp;

		// invalidate the old struct
		// (kind of a poor-mans unique_pointer)
		((CompressorDecompressor&) old).comp = nullptr;
		((CompressorDecompressor& ) old).decomp = nullptr;

	}

};


#endif /* COMPDECOMP_H_ */
