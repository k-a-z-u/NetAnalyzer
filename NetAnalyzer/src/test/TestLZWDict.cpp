/*
 * TestLZWDict.cpp
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */




#ifdef WITH_TESTS

#include "Test.h"

#include "../compression/lzwDict/LZWDict.h"
#include "../compression/lzwDict/LZWDictEncoder.h"
#include "../compression/lzwDict/LZWDictDecoder.h"
#include "../compression/lzwDict/LZWDictStream.h"

/*

TEST(LZWDict, init) {

	LZWDict dict;
	LZWWord word;

	// check each initial character
	for (uint32_t i = 0; i <= 0xFF; ++i) {
		word.clear();
		word.add(i);
		uint32_t idx = dict.getIdx(word);
		ASSERT_EQ(i, idx);
	}

	// check something else
	word.clear();
	word.add(1); word.add(2); word.add(3);
	ASSERT_EQ(LZW_DICT_NOT_FOUND, dict.getIdx(word));

}
 */

/*
 * this test was for the old hash-set
 * implement a test for the new tree??
TEST(LZWDict, append1) {

	LZWDict dict;

	std::string str = "the mother, the sister, the father";
	dict.append(str);

	ASSERT_EQ(257, dict.getIdx( LZWWord("th")));
	ASSERT_EQ(258, dict.getIdx( LZWWord("e ")) );
	ASSERT_EQ(259, dict.getIdx( LZWWord("mo")) );
	ASSERT_EQ(260, dict.getIdx( LZWWord("the")) );
	ASSERT_EQ(261, dict.getIdx( LZWWord("r,")) );
	ASSERT_EQ(262, dict.getIdx( LZWWord(" t")) );
	ASSERT_EQ(263, dict.getIdx( LZWWord("he")) );
	ASSERT_EQ(264, dict.getIdx( LZWWord(" s")) );
	ASSERT_EQ(265, dict.getIdx( LZWWord("is")) );
	ASSERT_EQ(266, dict.getIdx( LZWWord("te")) );
	ASSERT_EQ(267, dict.getIdx( LZWWord("r, ")) );
	ASSERT_EQ(268, dict.getIdx( LZWWord("the ")) );
	ASSERT_EQ(269, dict.getIdx( LZWWord("fa")) );
	ASSERT_EQ(270, dict.getIdx( LZWWord("ther")) );


	ASSERT_TRUE( LZWWord("th").equals(dict.getWord(257)) );
	ASSERT_TRUE( LZWWord("e ").equals(dict.getWord(258)) );
	ASSERT_TRUE( LZWWord("mo").equals(dict.getWord(259)) );
	ASSERT_TRUE( LZWWord("the").equals(dict.getWord(260)) );
	ASSERT_TRUE( LZWWord("r,").equals(dict.getWord(261)) );
	ASSERT_TRUE( LZWWord(" t").equals(dict.getWord(262)) );
	ASSERT_TRUE( LZWWord("he").equals(dict.getWord(263)) );
	ASSERT_TRUE( LZWWord(" s").equals(dict.getWord(264)) );
	ASSERT_TRUE( LZWWord("is").equals(dict.getWord(265)) );
	ASSERT_TRUE( LZWWord("te").equals(dict.getWord(266)) );
	ASSERT_TRUE( LZWWord("r, ").equals(dict.getWord(267)) );
	ASSERT_TRUE( LZWWord("the ").equals(dict.getWord(268)) );
	ASSERT_TRUE( LZWWord("fa").equals(dict.getWord(269)) );
	ASSERT_TRUE( LZWWord("ther").equals(dict.getWord(270)) );

}
*/
/*
TEST(LZWDict, hashSet1) {

	//LZWHashSet set;
	uint32_t cnt;

	for (uint32_t c = 0; c < 256; ++c) {

		LZWWord word;

		for (uint32_t i = 0; i < 16; ++i) {
			word.add(c);
			set.add(word, cnt);
			ASSERT_EQ(cnt, set.get(word));
			++cnt;
		}

	}

	LZWWord w3("L,");
	LZWWord w4;
	w4.add('L');
	w4.add(',');
	set.add(w3, 1);
	ASSERT_EQ(1, set.get(w3));
	ASSERT_EQ(1, set.get(w4));


}
*/

TEST(LZWDict, words) {

	LZWWord w1;
	LZWWord w2;

	ASSERT_TRUE(w1.equals(w2));
	ASSERT_TRUE(w2.equals(w1));

	for (uint32_t i = 0; i < 16; ++i) {

		w1.add(i);

		ASSERT_FALSE(w1.equals(w2));
		ASSERT_FALSE(w2.equals(w1));

		w2.add(i);

		ASSERT_TRUE(w1.equals(w2));
		ASSERT_TRUE(w2.equals(w1));

	}

}

TEST(LZWDict, words2) {

	LZWWord w1("123abc");
	LZWWord w2("123ABC");

	ASSERT_FALSE(w1.equals(w2));

	LZWWord w3(w1);
	ASSERT_TRUE(w3.equals(w1));
	ASSERT_FALSE(w3.equals(w2));

}

TEST(LZWDict, treeInit) {

	LZWDict d;

	const LZWTreeNode* n = d.getEncTree();

	for (uint32_t i = 0; i <= 0xFF; ++i) {
		ASSERT_NOT_NULL(n->nodes[i]);
		ASSERT_EQ(i, n->nodes[i]->idx);
	}

}

TEST(LZWDict, encodeDecode) {


	std::string str =
			"the quick brown fox jumps over the lazy dog. the father, the monther, the sister."
			"Lorem ipsum dolor sit amet";

	// initialize dictionary
	LZWDict dict;
	dict.append(str);

	// create encoder and decoder
	LZWDictEncoder enc(dict);
	LZWDictDecoder dec(dict);

	LZWDictStream stream(1024*1024);
	DataBuffer out;
	out.resize(1024*1024);
	std::string reDecoded;

	// encode
	enc.append( (uint8_t*)str.c_str(), str.length(), stream);
	enc.close(stream);

	// decode 2 bytes at once
	const uint8_t* data = stream.getData();
	for (uint32_t i = 0; i < stream.getNumUsed(); i+=2) {
		dec.append(data, 2, out);
		data += 2;
	}

	// check
	DataBufferData dbd = out.get();
	reDecoded = std::string( (const char*)dbd.data, dbd.length);
	std::cout << reDecoded << std::endl;
	ASSERT_EQ(str.length(), dbd.length);
	ASSERT_STREQ(str.c_str(), std::string( (const char*)dbd.data, dbd.length).c_str());

	// decode burst
	LZWDictDecoder dec2(dict);
	data = stream.getData();
	DataBuffer db2;
	db2.resize(1024*1024);
	dec2.append(data, stream.getNumUsed(), db2);

	// check
	DataBufferData dbd2 = db2.get();
	reDecoded = std::string( (const char*)dbd2.data, dbd2.length);
	std::cout << reDecoded << std::endl;
	ASSERT_EQ(str.length(), dbd2.length);
	ASSERT_STREQ(str.c_str(), reDecoded.c_str());

}


TEST(LZWDict, Performance) {



	const char* toEncode = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod "
			"tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam "
			"et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem "
			"ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy "
			"eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos "
			"et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus "
			"est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam "
			"nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero "
			"eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata "
			"sanctus est Lorem ipsum dolor sit amet. "
			""
			"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel "
			"illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui "
			"blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem "
			"ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt "
			"ut laoreet dolore magna aliquam erat volutpat. "
			""
			"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ";

	LZWDict dict;
	LZWDictEncoder enc(dict);
	LZWDictDecoder dec(dict);

	std::string toEncodeStr = toEncode;
	uint32_t toEncodeLength = toEncodeStr.length();

	LZWDictStream encoded(8*1024*1024);
	DataBuffer decoded;
	decoded.resize(8*1024*1024);

	uint32_t numRuns = 1024*64;

	// create the dictionary once
	dict.append(toEncodeStr);

	//exit(0);

	// now encode it several times
	uint64_t timeEnc = 0;
	uint64_t timeDec = 0;
	uint64_t decodedSize = 0;
	for (uint32_t i = 0; i < numRuns/1024; ++i) {

		// encode and track time
		uint64_t s1 = getTimeStampUS();
		for (uint32_t j = 0; j < 1024; ++j) {
			enc.append( (uint8_t*) toEncode, toEncodeLength, encoded);
		}
		enc.close(encoded);
		uint64_t e1 = getTimeStampUS();
		timeEnc += e1-s1;

		// decode and track time
		uint64_t s2 = getTimeStampUS();
		//for (uint32_t j = 0; j < 1024; ++j) {
			dec.append( encoded.getData(), encoded.getNumUsed(), decoded);
		//}
		uint64_t e2 = getTimeStampUS();
		timeDec += e2-s2;

		encoded.clear();

		decodedSize += decoded.bytesUsed();
		decoded.clear();

	}

	// show speed
	uint32_t bytesEncoded = toEncodeStr.length() * numRuns;
	float speedEnc = (bytesEncoded) / (timeEnc / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	float speedDec = (bytesEncoded) / (timeDec / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	std::cout << "speed (encode): " << speedEnc << " MB/s" << std::endl;
	std::cout << "speed (decode): " << speedDec << " MB/s" << std::endl;

	// sanity check
	ASSERT_EQ(toEncodeLength*numRuns, decodedSize);


}



#endif
