/*
 * HTTPDecoder.cpp
 *
 *  Created on: Apr 10, 2013
 *      Author: kazu
 */

#include "HTTPDecoder.h"
#include "HTTPHeader.h"
#include "HTTPListener.h"
#include "HTTPStream.h"
#include "HTTPDecoderStats.h"
#include "../network/Packet.h"
#include "../compression/Data.h"
#include "../compression/GZipDecompress.h"
#include "../Helper.h"
#include "../Debug.h"
#include "convInt.h"
#include "convHex.h"

#define DBG_HTTP		"HTTP" << this


HTTPDecoder::HTTPDecoder() {
	;
}

HTTPDecoder::~HTTPDecoder() {
	;
}

void HTTPDecoder::addListener(HTTPListener& listener) {
	listeners.push_back(&listener);
}

void HTTPDecoder::onOpen() {
	debug(DBG_HTTP, DBG_LVL_INFO, "open");
}

void HTTPDecoder::onClose() {
	debug(DBG_HTTP, DBG_LVL_INFO, "close");

	// update "numOK" if this was a "connection-close" request with payload
	if (stream.in.state == HTTP_STREAM_WAITING_FOR_UNKNOWN_CONTENT) {
		++((stream.in.isRequest) ? (stats.requests.numOK) : (stats.responses.numOK));
	}
	if (stream.out.state == HTTP_STREAM_WAITING_FOR_UNKNOWN_CONTENT) {
		++((stream.out.isRequest) ? (stats.requests.numOK) : (stats.responses.numOK));
	}

	// ensure the two buffers are empty!
	while (check(stream.in, stream.out)) {;}
	while (check(stream.out, stream.in)) {;}

	// sanity check
	if (!stream.in.buffer.empty()) {
		debug(DBG_HTTP, DBG_LVL_WARN, "TCP-connection closed, but HTTP-input-buffer not empty! coding flaw OR client aborted transfer.");
	}
	if (!stream.out.buffer.empty()) {
		debug(DBG_HTTP, DBG_LVL_WARN, "TCP-connection closed, but HTTP-output-buffer not empty! coding flaw OR client aborted transfer.");
	}

	// inform listeners
	for (auto it : listeners) {it->onHttpClose(*this);}

}

void HTTPDecoder::onDataFromInitiator(const Payload& data) {
	stream.in.buffer.append(data.data, data.length);
	//std::cerr << DBG_HTTP << std::endl;
	//stream.in.buffer.dump(std::cerr);
	while (check(stream.in, stream.out)) {;}
}


void HTTPDecoder::onDataToInitiator(const Payload& data) {
	stream.out.buffer.append(data.data, data.length);
	//std::cerr << DBG_HTTP << std::endl;
	//stream.out.buffer.dump(std::cerr);
	while (check(stream.out, stream.in)) {;}
}


bool HTTPDecoder::check(HTTPStreamDir& stream, HTTPStreamDir& otherDirection) {

	// currently: nothing to do
	if (stream.buffer.empty()) {return false;}

	// the stats object to work on for this stream's direction;
	HTTPDecoderStatsDir* curStats = (stream.isRequest) ? (&stats.requests) : (&stats.responses);



	// if the stream is waiting for a header, check if the buffer contains this header
	if (stream.state == HTTP_STREAM_WAITING_FOR_HEADER) {

		// check whether the buffer contains a header-end. if not -> skip
		int32_t pos = stream.buffer.indexOf( "\r\n\r\n" );
		if (pos == -1) {return false;}

		// sanity check
		if (pos < 14 || pos > 32*1024) {

			// all following traffic will be ignored (both directions!)
			stream.state = HTTP_STREAM_CORRUPTED;
			otherDirection.state = HTTP_STREAM_CORRUPTED;

			// log
			debug(DBG_HTTP, DBG_LVL_ERR, "strange header size. marking HTTP-stream as corrupted!");
			//stream.buffer.dump(std::cerr);

			return false;

		}


		debug(DBG_HTTP, DBG_LVL_INFO, "received header. length is: " << (pos + 4));

		// reset the stream to store the next header
		stream.reset();

		// get the header from the buffer.
		// this data will be removed from the header when this method ends and data is destructed
		DataBufferData data = stream.buffer.get(pos + 4);

		// create a string and a new header-object
		std::string header( (const char*) data.data, data.length );
		stream.header = new HTTPHeader();
		bool headerOK = stream.header->loadFromString(header);

		// if header-parsing failed (e.g. because of strange server behavior, skip everything
		// and mark BOTH directions as corrupted.
		if (!headerOK) {
			debug(DBG_HTTP, DBG_LVL_ERR, "header parsing failed. marking this connection as corrupted!");
			stream.state = HTTP_STREAM_CORRUPTED;
			otherDirection.state = HTTP_STREAM_CORRUPTED;
			return false;
		}

		// free the data
		data.free();

		// is this a request or a response?
		stream.isRequest = stream.header->isRequestHeader();

		// UPDATE the stats object as we now know this stream's direction;
		curStats = (stream.isRequest) ? (&stats.requests) : (&stats.responses);

		if (!stream.header->getURL().empty()) {
			debug(DBG_HTTP, DBG_LVL_DEBUG, "URL: " << stream.header->getURL());
		}

		// track average header sizes
		curStats->avgHeaderSize.add(stream.header->getLength());

		// cleanup any previous compression instances
		#ifdef WITH_ZLIB
			if (stream.unzip != nullptr) {delete stream.unzip; stream.unzip = nullptr;}
		#endif
		
		// is the next content compressed?
		if (stream.header->has("content-encoding", "gzip")) {
			#ifdef WITH_ZLIB
				++curStats->numCompressed;
				debug(DBG_HTTP, DBG_LVL_DEBUG, "gzip compression found");
				stream.unzip = new GZipDecompress();
			#else
				debug(DBG_HTTP, DBG_LVL_WARN, "found gzip compressed HTTP stream, but compiled without gzip support");
				stream.state = HTTP_STREAM_CORRUPTED;
				otherDirection.state = HTTP_STREAM_CORRUPTED;
				return false;
			#endif
		}


		// adjust stats
		++curStats->numHeaders;

		// inform listeners
		if (stream.isRequest) {
			for (auto it : listeners) {it->onHttpRequest(*this, *stream.header);}
		} else {

			// FIXME
			// after several hours the response header is set, but the request header isn't
			// and the app will crash with a nullptr.. thus we will use this sanity check here..
			if (otherDirection.header == nullptr) {
				debug(DBG_HTTP, DBG_LVL_ERR, "response-header available, but request-header isn't! marking this connection as corrupted!");
				stream.state = HTTP_STREAM_CORRUPTED;
				otherDirection.state = HTTP_STREAM_CORRUPTED;
				return false;
			}

			uint32_t latency = stream.header->getTimestamp() - otherDirection.header->getTimestamp();
			for (auto it : listeners) {it->onHttpResponse(*this, *otherDirection.header, *stream.header, latency);}

		}

		// check for latency information (if this is a response)
		//if (!stream.isRequest) {checkLatency();}


		// decide how to proceed
		if (stream.headerOnlyRequest) {
			stream.headerOnlyRequest = false;
			stream.state = HTTP_STREAM_WAITING_FOR_HEADER;
			debug(DBG_HTTP, DBG_LVL_INFO, "request was HEAD -> don't load any payload and proceed with next header");
			++curStats->numWithoutPayload;
			++curStats->numOK;

		} else if (stream.header->containsKey("content-length")) {
			stream.contentLength = strToInt(stream.header->get("content-length"));
			stream.state = HTTP_STREAM_WAITING_FOR_CONTENT;
			debug(DBG_HTTP, DBG_LVL_INFO, "normal content -> waiting for " << stream.contentLength << " bytes of payload");
			++curStats->numNormal;

		} else if (stream.header->has("transfer-encoding", "chunked")) {
			stream.state = HTTP_STREAM_WAITING_FOR_CHUNK_SIZE;
			debug(DBG_HTTP, DBG_LVL_INFO, "chunked content -> waiting for size-info of the next chunk");
			++curStats->numChunked;

		} else if (stream.header->has("connection", "close")) {
			stream.state = HTTP_STREAM_WAITING_FOR_UNKNOWN_CONTENT;
			debug(DBG_HTTP, DBG_LVL_INFO, "unknown content-length -> reading until connection closed");
			++curStats->numUnkownSize;

		} else {
			stream.state = HTTP_STREAM_WAITING_FOR_HEADER;
			debug(DBG_HTTP, DBG_LVL_INFO, "header without payload -> waiting for next header");
			++curStats->numWithoutPayload;
			++curStats->numOK;

		}

		// if only the HEAD(er) has been requested
		// the response-stream must not retrieve payload after the next header
		otherDirection.headerOnlyRequest = (stream.header->getMethod() == HTTP_METHOD_HEAD);

	}


	// receive "endless" content until connection: close
	if (stream.state == HTTP_STREAM_WAITING_FOR_UNKNOWN_CONTENT) {

		// skip when buffer is empty
		if (stream.buffer.empty()) {return false;}

		// get current buffer content
		DataBufferData data = stream.buffer.get();

		debug(DBG_HTTP, DBG_LVL_INFO, "buffer returned: " << data.length);

		// handle the incoming data
		handleData(stream, otherDirection, data);

		// increment receive-counter
		stream.contentReceived += data.length;

		// cleanup
		data.free();

	}

	// header received. is chunked. waiting for size-information of next chunk
	if (stream.state == HTTP_STREAM_WAITING_FOR_CHUNK_SIZE) {

		// we need at least 2 bytes to proceed
		if (stream.buffer.bytesUsed() < 2) {return false;}

		// wait for chunk-size info
		int pos = stream.buffer.indexOf("\r\n");
		if (pos == -1) {return false;}

		// read chunk-size from stream
		DataBufferData data = stream.buffer.get(pos+2);

		// store chunk-size
		std::string size( (const char*) data.data, data.length-2);
		stream.chunkLength = hexStringToInt(size);

		debug(DBG_HTTP, DBG_LVL_INFO, "next chunk's size is: " << stream.chunkLength);

		// remove from the buffer
		data.free();

		// now start loading chunk data
		stream.state = HTTP_STREAM_WAITING_FOR_CHUNK_DATA;
		debug(DBG_HTTP, DBG_LVL_INFO, "waiting for " << stream.chunkLength << " bytes of chunk-data");

	}

	// header received. content's size is known. waiting for content.
	if (stream.state == HTTP_STREAM_WAITING_FOR_CONTENT) {

		// get current buffer content
		// will be removed from buffer when this method returns
		uint32_t maxSize = stream.contentLength - stream.contentReceived;
		DataBufferData data = stream.buffer.get(maxSize);

		debug(DBG_HTTP, DBG_LVL_INFO, "content: " << maxSize << " bytes to go. buffer returned: " << data.length);

		// handle the incoming data
		handleData(stream, otherDirection, data);

		// increment receive-counter
		stream.contentReceived += data.length;

		// free the data
		data.free();

		// content completely received?
		if (stream.contentReceived == stream.contentLength) {

			stream.contentLength = 0;
			stream.contentReceived = 0;
			stream.state = HTTP_STREAM_WAITING_FOR_HEADER;
			debug(DBG_HTTP, DBG_LVL_INFO, "content complete");
			debug(DBG_HTTP, DBG_LVL_INFO, "waiting for next header");

			// inform listener
			for (auto it : listeners) {it->onHttpDone(*this, *stream.header);}

			// update stats
			++curStats->numOK;

		}

	} else if (stream.state == HTTP_STREAM_WAITING_FOR_CHUNK_DATA) {

		// get current buffer content
		// will be removed from buffer when this method returns
		uint32_t maxSize = stream.chunkLength - stream.contentReceived;
		DataBufferData data = stream.buffer.get(maxSize);

		debug(DBG_HTTP, DBG_LVL_INFO, "chunk: " << maxSize << " bytes to go. buffer returned: " << data.length);

		// handle the incoming data
		handleData(stream, otherDirection, data);

		// increment receive-counter
		stream.contentReceived += data.length;

		// free the data
		data.free();

		// chunk completely received?
		if (stream.contentReceived == stream.chunkLength) {
			stream.contentReceived = 0;
			stream.state = HTTP_STREAM_WAITING_FOR_CHUNK_END;
			debug(DBG_HTTP, DBG_LVL_INFO, "chunk complete");
			debug(DBG_HTTP, DBG_LVL_INFO, "waiting for chunk's trailing \\r\\n");

		}

	}

	// chunk received. waiting for the trailing "\r\n"
	if (stream.state == HTTP_STREAM_WAITING_FOR_CHUNK_END) {

		// each chunk ends with "\r\n"
		// wait for these two bytes here
		if (stream.buffer.bytesUsed() < 2) {return false;}
		DataBufferData data = stream.buffer.get(2);
		debug(DBG_HTTP, DBG_LVL_INFO, "received chunk's trailer");

		// immediately clean-up
		data.free();

		// if this was the last chunk (current chunkLength == 0)
		// we continue with a header, else we continue with another cunk
		if (stream.chunkLength == 0) {

			stream.state = HTTP_STREAM_WAITING_FOR_HEADER;
			debug(DBG_HTTP, DBG_LVL_INFO, "last chunk -> waiting for next header");

			// inform listeners
			for (auto it: listeners) {it->onHttpDone(*this, *stream.header);}

			// update stats
			++curStats->numOK;

		} else {
			stream.state = HTTP_STREAM_WAITING_FOR_CHUNK_SIZE;
			debug(DBG_HTTP, DBG_LVL_INFO, "waiting for another chunk's size");

		}

		// debug
		//stream.buffer.dump();
		stream.chunkLength = 0;
		stream.contentReceived = 0;

	}

	// corrupted? skip everything and wait for close()
	if (stream.state == HTTP_STREAM_CORRUPTED) {
		return false;
	}

	// everything fine. buffer might contain more usable data. return true -> try again
	return true;

}

void HTTPDecoder::handleData(HTTPStreamDir& stream, const HTTPStreamDir& streamOther, DataBufferData& data) {

	// skip if data is empty
	if (data.length == 0) {return;}

	// do we need decompression??
	if (stream.unzip != nullptr) {
	
		#ifdef WITH_ZLIB

			// try to uncompress data
			std::vector<uint8_t> uncompressed;
			bool ret = stream.unzip->append({ data.data, data.length }, uncompressed);
			if (!ret) {
				++((stream.isRequest) ? (stats.requests.numDecompressErrors) : (stats.responses.numDecompressErrors));
			}

			// inform listeners
			if (stream.isRequest) {
				for (auto it : listeners) {it->onHttpRequestTraffic(*this, *stream.header, uncompressed.data(), uncompressed.size());}
			} else {
				for (auto it : listeners) {it->onHttpResponseTraffic(*this, *streamOther.header, *stream.header, uncompressed.data(), uncompressed.size());}
			}
		
		#endif

	} else {

		// inform listeners
		if (stream.isRequest) {
			for (auto it : listeners) {it->onHttpRequestTraffic(*this, *stream.header, data.data, data.length);}
		} else {
			for (auto it : listeners) {it->onHttpResponseTraffic(*this, *streamOther.header, *stream.header, data.data, data.length);}
		}

	}

}

struct HTTPDecoderStats HTTPDecoder::stats = HTTPDecoderStats();

const HTTPDecoderStats& HTTPDecoder::getStats() {return HTTPDecoder::stats;}
