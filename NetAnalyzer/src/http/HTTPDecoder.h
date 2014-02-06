/*
 * HTTPDecoder.h
 *
 *  Created on: Apr 10, 2013
 *      Author: kazu
 */

#ifndef HTTPDECODER_H_
#define HTTPDECODER_H_

#include "../network/processors/TCPStreamListener.h"
#include "../buffer/DataBuffer.h"
#include "HTTPStream.h"

#include <vector>

// forward declarations
class HTTPListener;
struct HTTPDecoderStats;




/**
 * this class scans for HTTP Requests and Responses within ONE
 * TCP-Connection (stream).
 */
class HTTPDecoder : public TCPStreamListener {
public:

	/** ctor */
	HTTPDecoder();

	/** dtor */
	virtual ~HTTPDecoder();

	void onOpen() override;

	void onClose() override;

	void onDataFromInitiator(const Payload& data) override;

	void onDataToInitiator(const Payload& data) override;

	/** add one listener to the HTTP-decoder */
	void addListener(HTTPListener& listener);


	/** get stats of all HTTP decoder classes */
	static const HTTPDecoderStats& getStats();

private:

	/** hidden copy ctor */
	HTTPDecoder(const HTTPDecoder&);

	/** check for one complete request within the provided stream's buffer */
	bool check(HTTPStreamDir& stream, HTTPStreamDir& otherDirection);

	/** check whether latency (request->response) values are available and provide them to the listener */
	void checkLatency();

	/** handle received data on the given directional stream */
	void handleData(HTTPStreamDir& stream, const HTTPStreamDir& streamOther, DataBufferData& data);


	/** bi-directional HTTP traffic */
	HTTPStream stream;

	/** the listeners to send the traffic to */
	std::vector<HTTPListener*> listeners;


	/** stats for all HTTP decoder classes */
	static HTTPDecoderStats stats;

};

#endif /* HTTPDECODER_H_ */
