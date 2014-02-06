/*
 * TestBitstream.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: kazu
 */


#ifdef WITH_TESTS

#include "../compression/own/BitstreamOut.h"
#include "../compression/own/BitstreamIn.h"
#include "Test.h"

#include <math.h>
#include <vector>
#include <sstream>

#define BS_ENTRY_SIZE		sizeof(BSO_TYPE)


TEST(Bitstream, Write) {

	uint8_t data [64];

	BitstreamOut out(data);

	out.append(0b1111, 4);
	out.append(0b0000, 4);
	out.append(0b10101010, 8);
	out.append(0b1111111100000000, 16);
	out.append(0b001100110001110010111011, 24);
	out.append(0b11, 2);
	out.append(0b000, 3);
	out.append(0b111, 3);

	ASSERT_EQ(0b11110000, data[0]);
	ASSERT_EQ(0b10101010, data[1]);
	ASSERT_EQ(0b11111111, data[2]);
	ASSERT_EQ(0b00000000, data[3]);
	ASSERT_EQ(0b00110011, data[4]);
	ASSERT_EQ(0b00011100, data[5]);
	ASSERT_EQ(0b10111011, data[6]);
	ASSERT_EQ(0b11000111, data[7]);

	ASSERT_EQ(8, out.getBytesAvailable());

}

TEST(Bitstream, WriteFlush) {

	uint8_t data [64];

	BitstreamOut out(data);

	out.append(0b1111, 4);
	out.append(0b00, 2);

	ASSERT_EQ(0, out.getBytesAvailable());

	out.close();

	ASSERT_EQ(BS_ENTRY_SIZE, out.getBytesAvailable());
	ASSERT_EQ(0b11110000, data[0]);

	if (BS_ENTRY_SIZE != 1) {
		ASSERT_EQ(0b00000000, data[BS_ENTRY_SIZE-1]);
	}

}

TEST(Bitstream, WriteRewind) {

	uint8_t data [64];

	BitstreamOut out(data);

	for (uint32_t i = 0; i < BS_ENTRY_SIZE; ++i) {
		out.append(0b111, 3);
		out.append(0b00000, 5);
	}

	ASSERT_EQ(BS_ENTRY_SIZE, out.getBytesAvailable());
	ASSERT_EQ(0b11100000, data[0]);

	out.append(0b00110011, 8);
	out.rewind();

	for (uint32_t i = 0; i < BS_ENTRY_SIZE-1; ++i) {
		out.append(0b111, 3);
		out.append(0b00000, 5);
	}

	ASSERT_EQ(BS_ENTRY_SIZE, out.getBytesAvailable());
	ASSERT_EQ(0b00110011, data[0]);
	ASSERT_EQ(0b11100000, data[1]);

}

TEST(Bitstream, Read) {

	uint8_t data[] = {0b00110001};

	BitstreamIn in(data, 1);

	ASSERT_EQ(8, in.getNumReadableBits());

	std::stringstream ss;
	for (uint32_t i = 0; i < 8; ++i) {
		ss << in.getBit();
	}

	ASSERT_STREQ("00110001", ss.str().c_str());

}

TEST(Bitstream, ReadReset) {

	uint8_t data[] = {0b00110001};

	BitstreamIn in(data, 1);

	ASSERT_EQ(8, in.getNumReadableBits());

	std::stringstream ss;
	for (uint32_t i = 0; i < 8; ++i) {ss << in.getBit();}

	ASSERT_STREQ("00110001", ss.str().c_str());

	data[0] = 0b01010101;
	in.reset();

	ss.str("");
	ss.clear();
	for (uint32_t i = 0; i < 8; ++i) {ss << in.getBit();}

	ASSERT_STREQ("01010101", ss.str().c_str());

}

TEST(Bitstream, WriteRead) {

	uint8_t data[1024];


	BitstreamOut out(data);
	out.append(0b00, 2);
	out.append(0b10101, 5);
	out.append(0b1111, 4);
	out.append(0b10101010, 8);
	out.append(0b0000000011111111, 16);
	out.append(0b0101010110101010, 16);
	out.append(0b000, 3);
	out.append(0b1100110011001100, 16);
	out.append(0b11110000, 8);

	out.close();

	uint32_t numBytes = ceil(9.75/BS_ENTRY_SIZE)*BS_ENTRY_SIZE;
	ASSERT_EQ(numBytes, out.getBytesAvailable());

	BitstreamIn in(data, numBytes);

	ASSERT_EQ(numBytes*8, in.getNumReadableBits());

	std::stringstream ss;
	uint32_t numBits = 78;
	uint32_t check = in.getNumReadableBits();
	while (numBits) {
		ss << in.getBit();
		--numBits;
		--check;
		ASSERT_EQ(check, in.getNumReadableBits());
	}

	ASSERT_STREQ("001010111111010101000000000111111110101010110101010000110011001100110011110000", ss.str().c_str());

}
/*
TEST(Bitstream, PerformanceWrite) {

	uint32_t size = 1024*1024*8;
	uint8_t* data = (uint8_t*) malloc (size);
	BitstreamOut str(data);

	uint32_t val = rand();
	uint64_t bits = 0;

	// write data
	uint64_t start = getTimeStampMS();
	for (uint32_t i = 0; i < 1024*1024*32; ++i) {

		str.append(val, 3);
		str.append(val, 6);
		str.append(val, 5);
		str.append(val, 9);
		str.append(val, 2);
		str.append(val, 11);
		bits += (3+6+5+9+2+11);

		if (bits / 8 > size - 8) {str.reset();}

	}
	uint64_t end = getTimeStampMS();

	// calculate speed
	uint64_t time = end-start;
	float speedWrite = (bits / 8.0f) / (time / 1000.0f) / 1024.0f / 1024.0f;
	std::cout << "speed (write): " << speedWrite << " MB/sec" << std::endl;

	delete data;

}
*/

/*
TEST(Bitstream, PerformanceRead) {

	uint32_t size = 1024*1024*8;
	uint8_t* data = (uint8_t*) malloc (size);
	BitstreamIn in(data, size);

	// read data
	uint32_t sum = 0;
	uint64_t bits = 0;
	uint64_t start = getTimeStampMS();
	for (uint32_t i = 0; i < 4; ++i) {
		for (uint32_t j = 0; j < size*8; ++j) {
			sum += in.getBit();
			++bits;
		}
		in.reset();
	}
	uint64_t end = getTimeStampMS();

	std::cout << sum << std::endl;

	// calculate speed
	uint64_t time = end-start;
	float speedRead = (bits / 8.0f) / (time / 1000.0f) / 1024.0f / 1024.0f;
	std::cout << "speed (read): " << speedRead << " MB/sec" << std::endl;

	delete data;

}
*/


#endif
