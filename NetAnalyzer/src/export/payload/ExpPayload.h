/*
 * ExpPayload.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef EXPPAYLOAD_H_
#define EXPPAYLOAD_H_

#include <map>
#include <string>
#include <iosfwd>

#include "../../misc/File.h"
#include "../EventExporter.h"
#include "../../http/HTTPListener.h"
#include "../../analyzer/source/NetSourceListener.h"

// forward declarations
class HTTPHeader;
class NetSource;


struct ExpPayContentType {

	/** where to store all payloads for this content type */
	File folder;

	/** the extension for all files of this content-type */
	std::string fileExt;

	/** the stream for global stats of this content type */
	std::ofstream* stream;

	ExpPayContentType() : folder(), fileExt(), stream(nullptr) {;}


};

/**
 * exports all sniffed HTTP-payloads by their content type
 */
class ExpPayload : public EventExporter, public HTTPListener, public NetSourceListener {

public:

	ExpPayload();

	virtual ~ExpPayload();

	/** add one content-type to export all payloads for */
	void addContentType(const std::string& contentType, const std::string& fileExt);


	void onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& reqHeader) override;

	void onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, uint32_t latency) override;

	void onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) override;

	void onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) override;

	void onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) override;

	void onHttpClose(const HTTPDecoder& dec) override;


	void onNewTcpConnection(HTTPAnalyzer& analyzer, HTTPDecoder& dec) override;


protected:

	void visit(NetSource& src) override;

private:

	/** hidden copy ctor */
	ExpPayload(const ExpPayload&);

	/** the content-types to export the payload for */
	std::map<std::string, ExpPayContentType> contentTypes;

	/** open files we currently receive payload for */
	std::map<const HTTPDecoder*, std::ofstream*> openFiles;

	/** where to store files */
	File folder;

};

#endif /* EXPPAYLOAD_H_ */
