/*
 * Dict.h
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#ifndef DICT_H_
#define DICT_H_

#include <vector>
#include <cstdint>
#include <string>
#include "../Data.h"


struct DictElem {

	std::vector<uint8_t> data;

	uint8_t byte;

	/** ctor */
	DictElem() : data(), byte(0) {;}

	/** convert to data */
	Data getData() const {return Data(&data[0], data.size());}

	/** matches? */
	bool matches(const uint8_t* data, uint32_t length) const {
		if (this->data.size() > length) {return false;}
		uint32_t dataLength = this->data.size();
		for (uint32_t i = 2; i < dataLength; ++i) {
			if (this->data[i] != data[i]) {return false;}
		}
		return true;
	}

};

struct DictSlot {
	std::vector<DictElem> elems;
};

class Dict {
public:

	/** ctor */
	Dict();

	/** dtor */
	virtual ~Dict();

	/** add a new dictionary entry mapping the given string to the given byte */
	void addWord(const uint8_t* data, uint32_t length, uint8_t byte);

	/** convert byte back to string (decompress) */
	const Data getWord(uint8_t byte) const;

	/** convert string to byte (compress). returns true if dictionary contained string */
	bool getByte(const uint8_t* data, uint32_t length, uint8_t& byte, uint8_t& consumed) const;

private:

	/** for compression */
	DictSlot wordToByte[256][256];

	/** for decompression */
	DictElem byteToWord[256];

};

#endif /* DICT_H_ */
