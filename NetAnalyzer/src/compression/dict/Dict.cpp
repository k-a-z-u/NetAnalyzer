/*
 * Dict.cpp
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#include "Dict.h"

Dict::Dict() {
	// TODO Auto-generated constructor stub

}

Dict::~Dict() {
	// TODO Auto-generated destructor stub
}

#include <iostream>

void Dict::addWord(const uint8_t* data, uint32_t length, uint8_t byte) {

	if (length < 3) {return;}

	DictElem de;
	de.byte = byte;
	for (uint32_t i = 0; i < length; ++i) {de.data.push_back(data[i]);}
	wordToByte[data[0]][data[1]].elems.push_back(de);

	// reverse mapping
	byteToWord[byte] = de;

}

/** convert byte back to string (decompress) */
const Data Dict::getWord(uint8_t byte) const {
	return byteToWord[byte].getData();
}


/** convert string to byte (compress). returns true if dictionary contained string */
bool Dict::getByte(const uint8_t* data, uint32_t length, uint8_t& byte, uint8_t& consumed) const {


	//std::cout << (int) (data[0]) << ":" << (int) (data[1]) << std::endl;
	const DictSlot& slot = wordToByte[data[0]][data[1]];

	// not found
	if (slot.elems.empty()) {return false;}

	std::cout << "hit" << std::endl;


	// search
	for (const auto& elem : slot.elems) {
		if (elem.matches(data, length)) {
			byte = elem.byte;
			consumed = elem.data.size();
			return true;
		}
	}

	// not found
	return false;


}
