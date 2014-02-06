/*
 * TestDict.cpp
 *
 *  Created on: May 8, 2013
 *      Author: kazu
 */




#ifdef WITH_TESTS

#include "Test.h"

#include "../compression/dict/DictFlex.h"
#include "../compression/dict/DictEncoder.h"
#include "../buffer/DataBuffer.h"

TEST(Dict, Compress) {

	const char* dictStr = "quickfoxdogjumps";

	DictFlex dict;
	dict.setDict( (const uint8_t*) dictStr, strlen(dictStr) );

	const char* toEncode = "The quick brown fox jumps over the lazy dog123";

	DictEncStream stream(1024*1024);
	DictEncoder enc(&dict);

	enc.append( (const uint8_t*) toEncode, strlen(toEncode), stream);

	dumpHex(std::cout, stream.data, stream.getNumBytes());

	// check
	ASSERT_EQ(42, stream.getNumBytes());
	ASSERT_EQ(0xFF, stream.data[4]);
	ASSERT_EQ(0x00, stream.data[5]);
	ASSERT_EQ(0x05, stream.data[6]);

	ASSERT_EQ(0xFF, stream.data[14]);
	ASSERT_EQ(0x00, stream.data[15]);
	ASSERT_EQ(0x53, stream.data[16]);		// (offset 5) * 16 + (length 3)

	ASSERT_EQ(0xFF, stream.data[18]);
	ASSERT_EQ(0x00, stream.data[19]);
	ASSERT_EQ(0xB5, stream.data[20]);		// (offset 11) * 16 + (length 5)

	ASSERT_EQ(0xFF, stream.data[36]);
	ASSERT_EQ(0x00, stream.data[37]);
	ASSERT_EQ(0x83, stream.data[38]);		// (offset 8) * 16 + (length 3)



}

/** test empty value field within header */
/*
TEST(Dict, Performance) {

	const char* dictStr = "Loremipsumdolorsitametconsetetursadipscingelitrseddiamnonumyeirmodnonumyeirmodtemporinviduntutlaboreetdoloremagnaaliquyameratseddiamvoluptuablanditpraesentluptatumzzrildelenitaugueduisdoloretefeugaitnullafacilisi";

	DictFlex dict;
	dict.setDict( (const uint8_t*) dictStr, strlen(dictStr) );

	DictEncoder enc(&dict);

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


	const uint32_t numRuns = 1024*128;
	uint64_t timeEnc = 0;
	//uint64_t timeDec = 0;

	DictEncStream stream(1024*1024);

	const uint32_t toEncodeLength = strlen(toEncode);

	for (uint32_t i = 0; i < numRuns/1024; ++i) {

		// perform encoding
		uint64_t s1 = getTimeStampUS();

		for (uint32_t j = 0; j < 1024; ++j) {
			enc.append( (const uint8_t*) toEncode, toEncodeLength, stream );
			//std::cout << stream.bytesAvailable << ":" << toEncodeLength << std::endl;
			stream.reset();
		}

		uint64_t e1 = getTimeStampUS();
		timeEnc += e1-s1;

		//std::cout << "lol" << std::endl;

		// perform decoding
		//uint64_t s2 = getTimeStampUS();
		//uint64_t e2 = getTimeStampUS();
		//timeDec += e2-s2;

	}

	// show speed
	float speedEnc = (numRuns * toEncodeLength) / (timeEnc / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	//float speedDec = (numRuns * sizeof(toEncode)) / (timeDec / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	std::cout << "speed (encode): " << speedEnc << " MB/s" << std::endl;
	//std::cout << "speed (decode): " << speedDec << " MB/s" << std::endl;

}
*/

#endif
