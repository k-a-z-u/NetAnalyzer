/*
 * GUISummary.h
 *
 * contains summary data for the GUI
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef SUMMARY_H_
#define SUMMARY_H_

#include "../compression/own/HuffmanBuilder.h"
#include "other/WordAnalyzer.h"
#include "../Helper.h"
#include "Statistics.h"

#include <map>
#include <string>
#include <cstdint>

struct TCPReassemblerStats;

struct Compression {

	/** the total size of the compressed output */
	uint64_t compressedPayloadSize = 0;

	/** the time needed for compression */
	uint64_t compressedPayloadUs = 0;

	/** get the compression speed in bytes / second */
	uint32_t getCompressionSpeed(const uint64_t uncompressedSize) const {
		uint32_t compSpeed = (uncompressedSize) / (compressedPayloadUs / 1000.0 / 1000.0 + 0.00001);
		return compSpeed;
	}

	/** get the bytesSave / usedTime ratio in bytes / second */
	uint32_t getReductionSpeed(const uint64_t uncompressedSize) const {
		uint64_t bytesSafed = uncompressedSize - compressedPayloadSize;
		uint32_t compRedSpeed = (bytesSafed) / (compressedPayloadUs / 1000.0 / 1000.0 + 0.00001);
		return compRedSpeed;
	}

};

struct Decompression {

	/** size of the input data */
	uint64_t compressedPayloadSize = 0;

	/** size of the decompressed data */
	uint64_t decompressedPayloadSize = 0;

	/** the time needed for decompression */
	uint64_t decompressionUs = 0;

	/** get the decompression speed in bytes / second */
	uint32_t getDecompressionSpeed() const {
		uint32_t compSpeed = (decompressedPayloadSize) / (decompressionUs / 1000.0 / 1000.0 + 0.00001);
		return compSpeed;
	}

};

struct CompressorStats {

	/** stats for compression */
	struct Compression compression;

	/** stats for decompression */
	struct Decompression decompression;

};

struct HTTPStats {

	/** total number of responses */
	uint32_t numHeaders;

	/** header size of all responses */
	uint32_t sizeHeaders;


	/** number of responses with code 200 */
	uint32_t numHttpOK;

	/** number of responses with code 301 */
	uint32_t numHttpMoved;

	/** number of responses with code 304 */
	uint32_t numHttpNotModified;

	/** number of responses with code 404 */
	uint32_t numHttpNotFound;

	/** number of responses with code 500 */
	uint32_t numHttpIntError;

	/** number of responses with other response code */
	uint32_t numHttpOther;

	/** track latency statistics */
	Statistics latency;


	/** total payload */
	uint64_t sizePayload;

	/** stats for each compressor / decompressor */
	std::map<std::string, CompressorStats> comprStats;


	/** ctor */
	HTTPStats() : numHeaders(0), sizeHeaders(0),
			numHttpOK(0), numHttpMoved(0), numHttpNotModified(0), numHttpNotFound(0), numHttpIntError(0), numHttpOther(0),
			latency(),
			sizePayload(0) {;}

};


struct HTTPContentType {

	/** the content-type the statistics are tracked for */
	std::string contentType;

	/** the HTTP server (undistinguished) response statistics for this content-type */
	struct HTTPStats response;

	/** the HTTP server response statistics for (probably) static payloads of this content-type */
	struct HTTPStats responseStatic;

	/** the HTTP server response statistics for (probably) dynamic payloads of this content-type */
	struct HTTPStats responseDynamic;


	/** the HuffmanTree for this content type */
	HuffmanBuilder huffman;

	/** analyze payload for common words */
	WordAnalyzer words;

	/** ctor */
	HTTPContentType(const std::string& contentType) :
		contentType(contentType), huffman(true), words() {;}

	/** ctor */
	HTTPContentType() : huffman(true) {;}


};




/**
 * store all summary-data of the scanning process here
 */
struct Summary {

	/** the number of received packets */
	uint32_t receivedPackets;

	/** the number of received bytes (including protocol overhead) */
	uint64_t receivedBytes;

	/** the number of protocol-overhead bytes (packet-size - payload-size) */
	uint32_t receivedProtocolOverheadBytes;


	/** tcp stats */
	struct tcp_t {

		/** access stats of the TCP-processor */
		const TCPReassemblerStats* stats;

		tcp_t() : stats(nullptr) { }

	} tcp;

	/** http statistics */
	struct http_t {

		/** access to stats tracked within HTTPDecoder */
		const struct HTTPDecoderStats* stats;

		/** global HTTP-statistics for all server responses */
		struct HTTPStats response;

		/** global HTTP-statistics for all client requestes */
		struct HTTPStats request;

		std::map<std::string, struct HTTPContentType> byContentType;

		http_t() : stats(nullptr) { }

	} HTTP;

	/** ctor */
	Summary() : receivedPackets(0), receivedBytes(0), receivedProtocolOverheadBytes(0) { }

};


#endif /* SUMMARY_H_ */


