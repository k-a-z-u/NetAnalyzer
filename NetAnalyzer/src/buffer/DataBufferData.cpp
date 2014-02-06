/*
 * DataBufferData.cpp
 *
 *  Created on: May 25, 2013
 *      Author: kazu
 */




#include "DataBufferData.h"
#include "DataBuffer.h"

/**
 * TODO
 * this .cpp / function() is only needed to avoid cyclic include
 * between DataBuffer and DataBufferData
 */
void DataBufferData::free() {
	if (data != nullptr && length != 0) {
		buffer.remove(*this);
		data = nullptr;
		length = 0;
	}
}
