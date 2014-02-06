/*
 * TestHuffman.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: kazu
 */


#ifdef WITH_TESTS

#include "Test.h"
#include "../compression/own/HuffmanBuilder.h"
#include "../compression/own/HuffmanEncodeLUT.h"
#include "../compression/own/HuffmanTree.h"
#include "../compression/own/HuffmanEncoder.h"
#include "../compression/own/HuffmanDecoder.h"
#include "../compression/own/BitstreamOut.h"
#include "../compression/own/BitstreamIn.h"

#define TestHuffEnc(str)\
		enc->append( (uint8_t*)str, sizeof(str)-1, stream );\
		src += str;\


TEST(Huffman, counter) {

	HuffmanBuilder b(true);
	b.append((uint8_t*)"12345678", 8);
	b.append((uint8_t*)"1A3B5C7D", 8);

	const uint32_t* vals = b.getCounterValues();
	ASSERT_EQ(3, vals['1']);
	ASSERT_EQ(3, vals['3']);
	ASSERT_EQ(3, vals['5']);
	ASSERT_EQ(3, vals['7']);

	ASSERT_EQ(2, vals['2']);
	ASSERT_EQ(2, vals['4']);
	ASSERT_EQ(2, vals['6']);
	ASSERT_EQ(2, vals['8']);

	ASSERT_EQ(2, vals['A']);
	ASSERT_EQ(2, vals['B']);
	ASSERT_EQ(2, vals['C']);
	ASSERT_EQ(2, vals['D']);

}

/** perform compression -> decompression test */
TEST(Huffman, CompleteCompressDecompress) {

	// create builder with pre-defined tree
	HuffmanBuilder hb(true);
	std::string values = "3	1	1	1	1	1	1	1	1	113908	122109	1	1	68221	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	671252	19169	227050	61801	1368	1326	13014	13667	13092	13099	1669	1574	16444	119500	71296	177569	28233	35634	22550	24158	12359	12049	10788	9098	10359	14293	33257	28678	171777	126506	172023	3400	46	10589	7919	13409	18496	20633	10751	4177	11194	16100	727	3844	6775	8909	12276	6057	12586	429	8561	24317	17514	5050	3414	15626	1276	3178	3681	8907	299	8907	238	47084	1	322837	58858	169057	165074	478769	110306	87607	152216	322681	10822	52370	244708	128157	299384	184664	123919	8568	256726	274651	377494	119941	64456	62706	27222	30023	18389	4088	1300	4088	1	1	622	1	2	1	18	1	1	1	1	1	1	1	1	1	1	1	1	1	1	353	1	1	74	1	2	3	2	1	169	1	127	296	42	1	1	1	5974	1	25	1	1	12	3	42	2	1	1	1	1	1	1	1	1	1	1403	1	1	1	1	45	8652	1	1	1	1	1	135	16462	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	627	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1";
	hb.loadFromString(values, '\t');

	// create encoder and decoder
	HuffmanTree* tree = hb.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	HuffmanDecoder* dec = tree->getDecoder();

	uint8_t data[1024];
	BitstreamOut stream(data);

	// encode
	std::string src = "";

	// encode multiple pieces.
	//TestHuffEnc will send them to the encoder and append them to the 'src' string
	TestHuffEnc("Lorem ipsum dolor ");
	TestHuffEnc("sit amet, ");
	TestHuffEnc("consetetur sadipscing elitr, ");
	TestHuffEnc("sed diam!");

	// finalize
	enc->close(stream);

	// perform decoding
	BitstreamIn streamDec(data, stream.getBytesAvailable());
	std::vector<uint8_t> decoded;
	dec->append(streamDec, decoded);

	// assert
	ASSERT_EQ(src.length(), decoded.size());

	// perform validation
	for (uint32_t i = 0; i < decoded.size(); ++i) {
		std::cout << (char) decoded[i];
		ASSERT_EQ(src[i], decoded[i]);
	}

	delete tree;
	delete enc;
	delete dec;

}

/** perform compression -> decompression test */
TEST(Huffman, BlockCompressDecompress) {

	HuffmanBuilder hb(true);
	std::string values = "3	1	1	1	1	1	1	1	1	113908	122109	1	1	68221	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	671252	19169	227050	61801	1368	1326	13014	13667	13092	13099	1669	1574	16444	119500	71296	177569	28233	35634	22550	24158	12359	12049	10788	9098	10359	14293	33257	28678	171777	126506	172023	3400	46	10589	7919	13409	18496	20633	10751	4177	11194	16100	727	3844	6775	8909	12276	6057	12586	429	8561	24317	17514	5050	3414	15626	1276	3178	3681	8907	299	8907	238	47084	1	322837	58858	169057	165074	478769	110306	87607	152216	322681	10822	52370	244708	128157	299384	184664	123919	8568	256726	274651	377494	119941	64456	62706	27222	30023	18389	4088	1300	4088	1	1	622	1	2	1	18	1	1	1	1	1	1	1	1	1	1	1	1	1	1	353	1	1	74	1	2	3	2	1	169	1	127	296	42	1	1	1	5974	1	25	1	1	12	3	42	2	1	1	1	1	1	1	1	1	1	1403	1	1	1	1	45	8652	1	1	1	1	1	135	16462	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	627	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1";
	hb.loadFromString(values, '\t');

	// create encoder and decoder
	HuffmanTree* tree = hb.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	HuffmanDecoder* dec = tree->getDecoder();

	uint8_t data[1024];
	std::vector<uint8_t> decoded;

	BitstreamOut stream(data);
	BitstreamIn streamDec;

	// encode
	std::string src = "";

	// encode multiple pieces and decompress them
	// TestHuffEnc will send them to the encoder and append them to the 'src' string
	TestHuffEnc("Lorem ipsum dolor ");
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, decoded);
	stream.rewind();

	TestHuffEnc("sit amet, ");
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, decoded);
	stream.rewind();

	TestHuffEnc("consetetur sadipscing elitr, ");
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, decoded);
	stream.rewind();

	TestHuffEnc("sed diam!");
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, decoded);
	stream.rewind();

	// finalize
	enc->close(stream);
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, decoded);

	// assert
	ASSERT_EQ(src.length(), decoded.size());

	// perform validation
	std::stringstream ss;
	for (uint32_t i = 0; i < decoded.size(); ++i) {
		ss << (char) decoded[i];
	}

	ASSERT_EQ(src, ss.str().c_str());

	delete tree;
	delete enc;
	delete dec;

}

/** perform compression -> decompression test */
/*
TEST(Huffman, Performance) {

	HuffmanBuilder hb(true);
	std::string values = "3	1	1	1	1	1	1	1	1	113908	122109	1	1	68221	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	671252	19169	227050	61801	1368	1326	13014	13667	13092	13099	1669	1574	16444	119500	71296	177569	28233	35634	22550	24158	12359	12049	10788	9098	10359	14293	33257	28678	171777	126506	172023	3400	46	10589	7919	13409	18496	20633	10751	4177	11194	16100	727	3844	6775	8909	12276	6057	12586	429	8561	24317	17514	5050	3414	15626	1276	3178	3681	8907	299	8907	238	47084	1	322837	58858	169057	165074	478769	110306	87607	152216	322681	10822	52370	244708	128157	299384	184664	123919	8568	256726	274651	377494	119941	64456	62706	27222	30023	18389	4088	1300	4088	1	1	622	1	2	1	18	1	1	1	1	1	1	1	1	1	1	1	1	1	1	353	1	1	74	1	2	3	2	1	169	1	127	296	42	1	1	1	5974	1	25	1	1	12	3	42	2	1	1	1	1	1	1	1	1	1	1403	1	1	1	1	45	8652	1	1	1	1	1	135	16462	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	627	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1";
	hb.loadFromString(values, '\t');

	// create encoder and decoder
	HuffmanTree* tree = hb.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	HuffmanDecoder* dec = tree->getDecoder();

	uint8_t data[4096];
	std::vector<uint8_t> decoded;

	BitstreamOut stream(data);
	BitstreamIn streamDec;

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
	uint64_t timeDec = 0;
	std::vector<uint8_t> dst;
	dst.reserve(16384);

	const uint32_t toEncodeLength = strlen(toEncode);

	for (uint32_t i = 0; i < numRuns; ++i) {

		// perform encoding
		uint64_t s1 = getTimeStampUS();
		enc->append( (uint8_t*) toEncode, toEncodeLength, stream);
		uint32_t numBytes = stream.getBytesAvailable();
		stream.rewind();
		uint64_t e1 = getTimeStampUS();
		timeEnc += e1-s1;

		// perform decoding
		uint64_t s2 = getTimeStampUS();
		streamDec.wrap(data, numBytes);
		dec->append(streamDec, dst);
		uint64_t e2 = getTimeStampUS();
		timeDec += e2-s2;

	}

	// finalize
	enc->close(stream);
	streamDec.wrap(data, stream.getBytesAvailable());
	dec->append(streamDec, dst);

	// show speed
	float speedEnc = (numRuns * toEncodeLength) / (timeEnc / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	float speedDec = (numRuns * toEncodeLength) / (timeDec / 1000.0f / 1000.0f) / 1024.0f / 1024.0f;
	std::cout << "speed (encode): " << speedEnc << " MB/s" << std::endl;
	std::cout << "speed (decode): " << speedDec << " MB/s" << std::endl;

	// assert everything is decoded
	ASSERT_EQ(numRuns * toEncodeLength, dst.size());


	delete tree;
	delete enc;
	delete dec;

}

*/




/** -------------------------------- OLD -------------------------------- */

/*
TEST(Huffman, CompRate1) {

	HuffmanBuilder b(false);
	b.append((uint8_t*)"11111111111111111111111111111111", 32);
	b.append((uint8_t*)"2", 1);

	HuffmanTree* tree = b.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	const HuffmanEncodeLUT& lut = enc->getLUT();
	const uint32_t* vals = b.getCounterValues();

	ASSERT_EQ(32, vals['1']);
	ASSERT_EQ(1, (uint32_t) lut.byte['1'].numBits);
	ASSERT_EQ(1, (uint32_t) lut.byte['2'].numBits);
	ASSERT_EQ(12, b.getCompressionRate());

	delete enc;
	delete tree;

}

TEST(Huffman, CompRate2) {

	HuffmanBuilder b(false);
	b.append((uint8_t*)"11111111111111111111111111111111", 32);
	b.append((uint8_t*)"22222222222222222222222222222222", 32);

	HuffmanTree* tree = b.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	const HuffmanEncodeLUT& lut = enc->getLUT();
	const uint32_t* vals = b.getCounterValues();

	ASSERT_EQ(32, vals['1']);
	ASSERT_EQ(32, vals['2']);
	ASSERT_EQ(1, (uint32_t) lut.byte['1'].numBits);
	ASSERT_EQ(1, (uint32_t) lut.byte['2'].numBits);
	ASSERT_EQ(12, b.getCompressionRate());

	delete enc;
	delete tree;

}

TEST(Huffman, CompRate3) {

	HuffmanBuilder b(false);
	b.append((uint8_t*)"11111111111111111111111111111111", 32);
	b.append((uint8_t*)"22222222222222222222222222222222", 32);
	b.append((uint8_t*)"33333333333333333333333333333333", 32);

	HuffmanTree* tree = b.getTree();
	HuffmanEncoder* enc = tree->getEncoder();
	const HuffmanEncodeLUT& lut = enc->getLUT();
	const uint32_t* vals = b.getCounterValues();

	ASSERT_EQ(32, vals['1']);
	ASSERT_EQ(32, vals['2']);
	ASSERT_EQ(32, vals['3']);
	ASSERT_EQ(1, (uint32_t) lut.byte['1'].numBits);
	ASSERT_EQ(2, (uint32_t) lut.byte['2'].numBits);
	ASSERT_EQ(2, (uint32_t) lut.byte['3'].numBits);
	ASSERT_EQ(20, b.getCompressionRate());

	delete enc;
	delete tree;

}
 */

#endif
