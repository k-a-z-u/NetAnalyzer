/*
 * HTTPStream.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#include "HTTPStream.h"
#include "HTTPHeader.h"
#include "../compression/GZipDecompress.h"

HTTPStreamDir::HTTPStreamDir() :
	header(nullptr), state(HTTP_STREAM_WAITING_FOR_HEADER), contentLength(0),
	chunkLength(0), contentReceived(0), headerOnlyRequest(false), isRequest(false), unzip(nullptr) {
	;
}

HTTPStreamDir::~HTTPStreamDir() {
	if (header != nullptr)	{delete header; header = nullptr;}
	#ifdef WITZ_ZLIB
		if (unzip != nullptr)	{delete unzip; unzip = nullptr;}
	#endif
}


void HTTPStreamDir::reset() {
	if (header != nullptr)	{delete header; header = nullptr;}
	#ifdef WITH_ZLIB
		if (unzip != nullptr)	{delete unzip; unzip = nullptr;}
	#endif
}

