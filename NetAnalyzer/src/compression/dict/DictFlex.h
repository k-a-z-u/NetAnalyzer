/*
 * DictFlex.h
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#ifndef DICTFLEX_H_
#define DICTFLEX_H_


#include <cstdint>
#include "opt.h"


//#define DICT_IDX(b1, b2, b3)	index[ (b1 << 16) | (b2 << 8) | (b3 << 0) ]
#define DICT_IDX( ptr )			index[ *((uint32_t*) ptr) & 0xFFFFFF ]


#define DICT_IDX_SIZE			((256UL * 256UL * 256UL) * sizeof(uint32_t))
#define DICT_MAX_MATCH_SIZE		(16+2)






#include <iostream>


class DictFlex {
public:

	DictFlex();



	~DictFlex();

	/** set the dictionaries content */
	void setDict(const uint8_t* data, uint32_t length);

	/** try to find longest matching sequence from dictionary */
	inline void getCode(const uint8_t* data, register const uint32_t length, register uint32_t& offset, register uint32_t& consumed) const {

		// check all index-slots
		//for (uint32_t slot = 0; slot < DICT_IDX_SLOTS; ++slot) {

			// get next starting sequence to check for their matching length
			offset =  DICT_IDX( &data[0] );

			// all sequences checked??
			if ( offset == 0xFFFFFFFF ) {return;}

			// the number of bytes available at the given index
			// = length until dictionaries end
			//const uint32_t dictSizeRemain = this->length - offset;

			// check matching length
			consumed = 3;
			while ( consumed < length && consumed <= DICT_MAX_MATCH_SIZE ) {

				if ( data[consumed] != this->data[consumed+offset] ) {break;}
				++consumed;

			}

		//}

	}



private:

	DictFlex(const DictFlex& src);

	/** index the dictionary */
	void createIndex();

	/** the dictionary data */
	uint8_t* data;

	/** the length of the dictionary */
	uint32_t length;

	/** index for faster dictionary access */
	uint32_t* index;

};

#endif /* DICTFLEX_H_ */
