/*
 * HTTPDecoderStats.h
 *
 *  Created on: Apr 15, 2013
 *      Author: kazu
 */

#ifndef HTTPDECODERSTATS_H_
#define HTTPDECODERSTATS_H_

#include <cstdint>
#include "../misc/Average.h"


/**
 * HTTP-Decoder stats for one direction
 */
struct HTTPDecoderStatsDir {

	/** total number of found headers */
	uint32_t numHeaders;

	/** the number of already compressed payloads */
	uint32_t numCompressed;

	/** the number of normal payloads (with content-length) */
	uint32_t numNormal;

	/** the number of chunked transmits */
	uint32_t numChunked;

	/** the number of transmits with unknown content-length (connection: close) */
	uint32_t numUnkownSize;


	/** total number of completely processed HTTP-Parts (header + payload) */
	uint32_t numOK;

	/** number of HTTP-traffic without payload (header only) */
	uint32_t numWithoutPayload;

	/** the number of decompression errors that occurred */
	uint32_t numDecompressErrors;

	/** track the average header size */
	Average avgHeaderSize;

};

/**
 * store stats of the HTTPDecoder
 */
struct HTTPDecoderStats {

	/** stats for incoming requests */
	struct HTTPDecoderStatsDir requests;

	/** stats for outgoing responses */
	struct HTTPDecoderStatsDir responses;

	HTTPDecoderStats() {;}

};


#endif /* HTTPDECODERSTATS_H_ */
