/*
 * HTTPListener.h
 *
 * listen for HTTP events
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef HTTPLISTENER_H_
#define HTTPLISTENER_H_

#include <cstdint>

class HTTPHeader;
class HTTPDecoder;

class HTTPListener {

public:

	/** dtor */
	virtual ~HTTPListener() {;}

	/** a new HTTP-request has been received */
	virtual void onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& reqHeader) = 0;

	/** a new HTTP-response has been received */
	virtual void onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, uint32_t latency) = 0;

	/** a new request data-packet has been received. data will be INVALID when this method returns! */
	virtual void onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) = 0;

	/** a new response data-packet has been received. data will be INVALID when this method returns! */
	virtual void onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) = 0;

	/** the data-transmission (for the last header) is complete */
	virtual void onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) = 0;

	/** connection closed. nothing more following */
	virtual void onHttpClose(const HTTPDecoder& dec) = 0;

};


#endif /* HTTPLISTENER_H_ */
