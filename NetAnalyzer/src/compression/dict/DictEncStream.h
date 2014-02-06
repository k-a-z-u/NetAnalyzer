/*
 * DictEncStream.h
 *
 *  Created on: May 8, 2013
 *      Author: kazu
 */

#ifndef DICTENCSTREAM_H_
#define DICTENCSTREAM_H_


struct DictEncStream {

	/** the start of the buffer to write data to */
	uint8_t* data;

	/** the current position within buffer */
	uint8_t* head;

	DictEncStream(uint32_t size) : data(nullptr) {
		data = (uint8_t*) malloc(size);
		head = data;
	}

	~DictEncStream() {
		if (data != nullptr) {free(data);}
		head = nullptr;
	}

	/** reset this stream */
	void reset() {
		head = data;
	}

	/** get number of bytes available */
	uint32_t getNumBytes() {return head-data;}

private:

	/** hidden copy ctor */
	DictEncStream(const DictEncStream&);

};


#endif /* DICTENCSTREAM_H_ */
