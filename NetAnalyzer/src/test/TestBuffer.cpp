/*
 * TestBuffer.cpp
 *
 *  Created on: Apr 21, 2013
 *      Author: kazu
 */

#ifdef WITH_TESTS

#include "../buffer/DataBuffer.h"
#include "Test.h"

TEST(DataBuffer, Delete) {
	DataBuffer db;
	const char* str = "123";
	db.append((uint8_t*)str, 3);
	DataBufferData d0 = db.get(1);
	ASSERT_DATA_EQ("1", d0.data, 1);
	ASSERT_EQ(3, db.bytesUsed());
	d0.free();
	ASSERT_EQ(2, db.bytesUsed());
	DataBufferData d1 = db.get(1);
	ASSERT_DATA_EQ("2", d1.data, 1);
	d1.free();
	ASSERT_EQ(1, db.bytesUsed());
	DataBufferData d2 = db.get(1);
	ASSERT_DATA_EQ("3", d2.data, 1);
	ASSERT_EQ(1, db.bytesUsed());
	d2.free();
	ASSERT_EQ(0, db.bytesUsed());
	ASSERT_TRUE(db.empty());
	ASSERT_EQ(0, d2.data);
	ASSERT_EQ(0, d2.length);
}

TEST(DataBuffer, Append) {
	DataBuffer db;
	db.append((uint8_t*)"123", 3);
	DataBufferData d1 = db.get();
	ASSERT_DATA_EQ("123", d1.data, 3);
	d1.free();
	db.append((uint8_t*)"123", 3);
	db.append((uint8_t*)"AABBCC", 4);
	DataBufferData d2 = db.get();
	ASSERT_DATA_EQ("123AABB", d2.data, 7);
}

TEST(DataBuffer, Search1) {
	DataBuffer db;
	const char* str = "the quick brown fox...";
	db.append((uint8_t*)str, 22);
	ASSERT_EQ(10, db.indexOf("brown"));
}

TEST(DataBuffer, Search2) {
	DataBuffer db;
	const char* str = "the quick brown fox...";
	db.append((uint8_t*)str, 22);
	DataBufferData d = db.get();
	d.free();
	ASSERT_EQ(-1, db.indexOf("brown"));
}

TEST(DataBuffer, Search3) {
	DataBuffer db;
	const char* str = "brown";
	db.append((uint8_t*)str, 5);
	ASSERT_EQ(0, db.indexOf("brown"));
}

TEST(DataBuffer, Search4) {
	DataBuffer db;
	const char* str = "brow";
	db.append((uint8_t*)str, 4);
	ASSERT_EQ(-1, db.indexOf("brown"));
}

TEST(DataBuffer, GetEmpty) {
	DataBuffer db;
	DataBufferData data1 = db.get();
	ASSERT_EQ(0, data1.length);
	ASSERT_EQ(0, data1.data);
}

TEST(DataBuffer, GetMax) {
	DataBuffer db;
	db.append((uint8_t*) "1234", 4);
	DataBufferData data1 = db.get(8);
	ASSERT_EQ(4, data1.length);
	data1.free();
	ASSERT_TRUE(db.empty());
}

/** should not produce any mem-leaks */
TEST(DataBuffer, Cleanup) {

	{
		DataBuffer db;
		db.append((uint8_t*) "1234", 4);
	}

}

#endif

