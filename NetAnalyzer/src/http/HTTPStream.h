/*
 * HTTPStream.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef HTTPSTREAM_H_
#define HTTPSTREAM_H_

#include "../buffer/DataBuffer.h"

class HTTPHeader;
class GZipDecompress;


#define HTTP_STREAM_WAITING_FOR_HEADER			1
#define HTTP_STREAM_WAITING_FOR_CONTENT			2
#define HTTP_STREAM_WAITING_FOR_UNKNOWN_CONTENT	4
#define HTTP_STREAM_WAITING_FOR_CHUNK_SIZE		6
#define HTTP_STREAM_WAITING_FOR_CHUNK_DATA		7
#define HTTP_STREAM_WAITING_FOR_CHUNK_END		8
#define HTTP_STREAM_CORRUPTED					255


/**
 * describes an UNIdirectional HTTP-Stream and it's state
 * (e.g. "waiting for header to arrive")
 */
struct HTTPStreamDir {

	/** the temporal data-buffer */
	DataBuffer buffer;

	/** the HTTP-header. (nullptr until a header has been received) */
	HTTPHeader* header;

	/** store the current state of this stream. e.g. "waiting for header" */
	uint8_t state;


	/** store the length of the content, if specified within the header */
	uint32_t contentLength;

	/** store the length of the current chunk if "Transfer-Encoding: chunked" */
	uint32_t chunkLength;

	/** the number of content-bytes already received. */
	uint32_t contentReceived;


	/** the client sent a HEAD .. request -> skip payload on next response */
	bool headerOnlyRequest;


	/**
	 * the HTTP header will decide whether this is a stream for
	 * requests (client -> server) or responses (server -> client)
	 */
	bool isRequest;


	/** if the data is compressed, we need to uncompress it */
	GZipDecompress* unzip;


	/** ctor */
	HTTPStreamDir();

	/** dtor */
	~HTTPStreamDir();

	/** reset the stream to receive the next header. this will remove current header and GUnzip */
	void reset();

private:

	/** hidden copy ctor */
	HTTPStreamDir(const HTTPStreamDir&);


};


/**
 * combines bi-directional http traffic
 */
struct HTTPStream {

	/** incoming HTTP traffic */
	HTTPStreamDir in;

	/** outgoing HTTP traffic */
	HTTPStreamDir out;

};


#endif /* HTTPSTREAM_H_ */
