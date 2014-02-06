/*
 * HTTPAnalyzer.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#include "HTTPAnalyzer.h"
#include "Summary.h"
#include "../http/HTTPHeader.h"
#include "../compression/Data.h"
#include "../compression/Compressor.h"
#include "../compression/Decompressor.h"
#include "CompDecomp.h"
#include "../Debug.h"


HTTPAnalyzer::HTTPAnalyzer(Summary& summary) : summary(summary), statsByLatency(nullptr) {

	// bind analyzer to core 0
	// better for compressors!
	bindCurrentThreadToCore(0);

}

HTTPAnalyzer::~HTTPAnalyzer() {
	;
}

void HTTPAnalyzer::onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& reqHeader) {

	(void) dec;

	++summary.HTTP.request.numHeaders;
	summary.HTTP.request.sizeHeaders += reqHeader.getLength();

}

void HTTPAnalyzer::onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, uint32_t latency) {

	(void) dec;
	(void) reqHeader;

	// open compressors for following traffic
	openCompressors();

	// get the content type of the response
	const std::string& contentType = respHeader.getContentType();

	// set the name of the content-type (once)
	if (summary.HTTP.byContentType[contentType].contentType.empty()) {
		summary.HTTP.byContentType[contentType].contentType = contentType;
	}

	// distinguish between static and dynamic content here
	if (latency < staticDynamicThreshold) {
		statsByLatency = &summary.HTTP.byContentType[contentType].responseStatic;
	} else {
		statsByLatency = &summary.HTTP.byContentType[contentType].responseDynamic;
	}

	// skip out-of-range latencies
	if (latency <= maxLatency) {

		// increment global counter
		summary.HTTP.byContentType[contentType].response.latency.add(latency);

		// increment static/dynamic counter
		statsByLatency->latency.add(latency);

	}

	// update global statistics
	++summary.HTTP.response.numHeaders;
	summary.HTTP.response.sizeHeaders += respHeader.getLength();


	//FIXME: needed or not?!
	//if (header.containsKey("content-type")) {

	//FIXME: needed or not?!
	// allocate content-type
	//if (summary.HTTP.byContentType.find(contentType) == summary.HTTP.byContentType.end()) {
	//	summary.HTTP.byContentType[contentType] = HTTPContentType(contentType);
	//}

	// update content type statistics
	++summary.HTTP.byContentType[contentType].response.numHeaders;
	summary.HTTP.byContentType[contentType].response.sizeHeaders += respHeader.getLength();
	++statsByLatency->numHeaders;
	statsByLatency->sizeHeaders += respHeader.getLength();

	// update status-code statistics
	switch (respHeader.getCode()) {
	case 200:
		++summary.HTTP.byContentType[contentType].response.numHttpOK;
		++statsByLatency->numHttpOK;
		break;

	case 302:
		++summary.HTTP.byContentType[contentType].response.numHttpMoved;
		++statsByLatency->numHttpMoved;
		break;

	case 304:
		++summary.HTTP.byContentType[contentType].response.numHttpNotModified;
		++statsByLatency->numHttpNotModified;
		break;

	case 404:
		++summary.HTTP.byContentType[contentType].response.numHttpNotFound;
		++statsByLatency->numHttpNotFound;
		break;

	case 500:
		++summary.HTTP.byContentType[contentType].response.numHttpIntError;
		++statsByLatency->numHttpIntError;
		break;

	default:
		++summary.HTTP.byContentType[contentType].response.numHttpOther;
		++statsByLatency->numHttpOther;
		break;
	}

}

void HTTPAnalyzer::onHttpRequestTraffic(const HTTPDecoder&dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) {

	(void) dec;
	(void) reqHeader;
	(void) data;
	summary.HTTP.request.sizePayload += length;

}

void HTTPAnalyzer::onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) {

	(void) dec;
	(void) reqHeader;

	// skip empty payload
	if (!length) { return; }

	summary.HTTP.response.sizePayload += length;

	const std::string& contentType = respHeader.getContentType();

	// update global stats
	summary.HTTP.byContentType[contentType].response.sizePayload += length;
	summary.HTTP.byContentType[contentType].huffman.append(data, length);
	summary.HTTP.byContentType[contentType].words.append(data, length);
	//summary.HTTP.byContentType[contentType].lzwDict.append(data, length);

	// update static/dynamic stats
	statsByLatency->sizePayload += length;

	// perform compression on this payload (for all configured compressors)
	appendToCompressors(contentType, data, length);


}

void HTTPAnalyzer::onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) {

	(void) dec;

	if (!header.isRequestHeader()) {

		if (header.containsKey("content-type")) {
			const std::string& contentType = header.getContentType();
			if (summary.HTTP.byContentType.find(contentType) != summary.HTTP.byContentType.end()) {

			}

			// cleanup all compressors
			closeCompressors(contentType);

		}
	}

}

void HTTPAnalyzer::onHttpClose(const HTTPDecoder& dec) {

	(void) dec;

	// when underlying TCP-connection is closed, the HTTP decoder will be deleted,
	// thus this analyzer is rendered useless and will also be deleted
	delete this;

}

void HTTPAnalyzer::addCompressorDecompressor(Compressor* comp, Decompressor* decomp) {
	CompressorDecompressor pair(comp, decomp);
	compressors.push_back(pair);
}


void HTTPAnalyzer::openCompressors() {
	for (auto& pair : compressors) {

		// reset compressor
		pair.comp->reset();
		pair.comp->open();

		// reset decompressor (if any)
		if (pair.decomp) {
			pair.decomp->reset();
		}

	}

}

// TODO
// the code for close and append is almost identical
// refactor!!
void HTTPAnalyzer::appendToCompressors(const std::string& type, const uint8_t* data, uint32_t length) {

	for (auto& pair : compressors) {

		// Gather information about how well the data could have been compressed
		Data uncompressed(data, length);

		// buffer for compression and decompression
		std::vector<char> bufferComp;
		std::vector<uint8_t> bufferDecomp;

		// perform compression
		const auto startC = getTimeStampUS();
		pair.comp->append(uncompressed, bufferComp);
		const auto stopC = getTimeStampUS();
		const auto timeCompress = stopC - startC;

		// perform decompression (if configured for the current compressor)
		uint64_t timeDecompress = 0;
		if (pair.decomp) {
			Data d( (uint8_t*) &bufferComp[0], bufferComp.size());
			uint64_t startD = getTimeStampUS();
			pair.decomp->append(d, bufferDecomp);
			uint64_t stopD = getTimeStampUS();
			timeDecompress = stopD - startD;
		}

		// update variables
		size_t compressedSize = bufferComp.size();
		size_t decompressedSize = bufferDecomp.size();

		// update stats
		auto& comprStats = summary.HTTP.byContentType[type].response.comprStats[pair.comp->getName()];
		comprStats.compression.compressedPayloadSize += compressedSize;
		comprStats.compression.compressedPayloadUs += timeCompress;
		comprStats.decompression.compressedPayloadSize += compressedSize;
		comprStats.decompression.decompressedPayloadSize += decompressedSize;
		comprStats.decompression.decompressionUs += timeDecompress;

		auto& comprStatsDetail = statsByLatency->comprStats[pair.comp->getName()];
		comprStatsDetail.compression.compressedPayloadSize += compressedSize;
		comprStatsDetail.compression.compressedPayloadUs += timeCompress;
		comprStatsDetail.decompression.compressedPayloadSize += compressedSize;
		comprStatsDetail.decompression.decompressedPayloadSize += decompressedSize;
		comprStatsDetail.decompression.decompressionUs += timeDecompress;


	}
}

void HTTPAnalyzer::closeCompressors(const std::string& type) {

	for (auto& pair : compressors) {

		// buffer for compression and decompression
		std::vector<char> bufferComp;
		std::vector<uint8_t> bufferDecomp;

		// perform compression
		const auto startC = getTimeStampUS();
		pair.comp->close(bufferComp);
		const auto stopC = getTimeStampUS();
		uint64_t timeCompress = stopC - startC;

		// perform decompression (if configured for the current compressor)
		uint64_t timeDecompress = 0;
		if (pair.decomp) {
			Data d( (uint8_t*) &bufferComp[0], bufferComp.size());
			uint64_t startD = getTimeStampUS();
			pair.decomp->append(d, bufferDecomp);
			uint64_t stopD = getTimeStampUS();
			timeDecompress = stopD - startD;
		}

		// update variables
		size_t compressedSize = bufferComp.size();
		size_t decompressedSize = bufferDecomp.size();

		// update stats
		auto& comprStats = summary.HTTP.byContentType[type].response.comprStats[pair.comp->getName()];
		comprStats.compression.compressedPayloadSize += compressedSize;
		comprStats.compression.compressedPayloadUs += timeCompress;
		comprStats.decompression.compressedPayloadSize += compressedSize;
		comprStats.decompression.decompressedPayloadSize += decompressedSize;
		comprStats.decompression.decompressionUs += timeDecompress;

		auto& comprStatsDetail = statsByLatency->comprStats[pair.comp->getName()];
		comprStatsDetail.compression.compressedPayloadSize += compressedSize;
		comprStatsDetail.compression.compressedPayloadUs += timeCompress;
		comprStatsDetail.decompression.compressedPayloadSize += compressedSize;
		comprStatsDetail.decompression.decompressedPayloadSize += decompressedSize;
		comprStatsDetail.decompression.decompressionUs += timeDecompress;
	}

}


/** statics */
uint32_t HTTPAnalyzer::maxLatency = 5000;
uint32_t HTTPAnalyzer::staticDynamicThreshold = 50;

void HTTPAnalyzer::setMaxLatency(uint32_t max)				{HTTPAnalyzer::maxLatency = max;}
void HTTPAnalyzer::setStaticDynamicThreshold(uint32_t ms)	{HTTPAnalyzer::staticDynamicThreshold = ms;}
