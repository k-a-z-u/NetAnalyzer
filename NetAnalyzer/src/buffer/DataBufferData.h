/*
 * DataBufferData.h
 *
 *  Created on: May 25, 2013
 *      Author: kazu
 */

#ifndef DATABUFFERDATA_H_
#define DATABUFFERDATA_H_

#include <cstdint>

// forward declaration
class DataBuffer;

/** data from the data-buffer */
struct DataBufferData {

public:

	/** the data */
	uint8_t* data;

	/** the number of readable bytes */
	uint32_t length;

	/** ctor */
	DataBufferData(uint8_t* data, uint32_t length, DataBuffer& buffer) : data(data), length(length), buffer(buffer) {;}

	/** dtor */
	~DataBufferData() {free();}

	/** free this data from the buffer */
	void free();

private:

	/** the buffer this data belongs to */
	DataBuffer& buffer;

};


#endif /* DATABUFFERDATA_H_ */
