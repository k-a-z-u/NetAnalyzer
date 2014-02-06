/*
 * Data.h
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef DATA_H_
#define DATA_H_

#include <cstdint>

/**
 * struct to store data-buffers with a given length.
 * the struct will NOT free any data!
 */
struct Data {

	/** the accessible data */
	const uint8_t* data;

	/** the number of usable bytes */
	uint32_t length;


	/** ctor for empty object */
	Data() : data(0), length(0) {;}

	/** ctor with payload */
	Data(const uint8_t* data, uint32_t length) : data(data), length(length) {;}

	/** dtor */
	~Data() {;}

	/** set data and length */
	void set(const uint8_t* data, uint32_t length) {
		this->data = data;
		this->length = length;
	}

	/** mark as empty */
	void unset() {
		this->data = 0;
		this->length = 0;
	}

};

#endif /* DATA_H_ */
