/*
 * DictEncoder.h
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#ifndef DICTENCODER_H_
#define DICTENCODER_H_


#include <string>

#include "DictEncStream.h"

// forward declarations
class DictFlex;
class DataBuffer;


class DictEncoder {
public:

	/** ctor */
	DictEncoder(const DictFlex* dict);

	virtual ~DictEncoder();

	/** compress the given string into the databuffer */
	void append(const uint8_t* data, uint32_t length, DictEncStream& stream);

private:

	/** hidden copy ctor */
	DictEncoder(const DictEncoder&);

	/** the dictionary to use */
	const DictFlex* dict;

};

#endif /* DICTENCODER_H_ */
