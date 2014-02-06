/*
 * DictCompress.h
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#ifndef DICTCOMPRESS_H_
#define DICTCOMPRESS_H_

#include "Compressor.h"
#include "dict/DictFlex.h"
#include "dict/DictEncStream.h"


// forward declarations
class DictEncoder;

class DictCompress : public Compressor {

public:

	/** ctor */
	DictCompress(const DictFlex& dict);

	/** dtor */
	virtual ~DictCompress();

	Compressor* create() const override;

	bool open() override;

	//bool append(const Data& toCompress, size_t& compressedSize) override;

	//bool close(size_t& compressedSize) override;

	bool append(const Data& toCompress, std::vector<char>& result) override;

	bool close(std::vector<char>& result) override;

	void reset() override;

private:

	/** the dictionary encoder */
	DictEncoder* enc;

	/** the dict to use */
	const DictFlex& dict;

	/** the buffer to encode to */
	DictEncStream stream;

};

#endif /* DICTCOMPRESS_H_ */
