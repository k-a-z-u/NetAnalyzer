/*
 * ExpPlotLatency.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: kazu
 */

#include "ExpPlotLatency.h"

#include "../../analyzer/source/NetSource.h"
#include "../../http/HTTPHeader.h"
#include "../../http/HTTPDecoder.h"
#include "../../misc/File.h"
#include "../../misc/MyString.h"
#include "../../Debug.h"

#include <algorithm>
#include <fstream>

#define DBG_EXP_LATENCY		"expLat"


ExpPlotLatency::ExpPlotLatency() {
	debug(DBG_EXP_LATENCY, DBG_LVL_INFO, "creating latency-exporter");
}

ExpPlotLatency::~ExpPlotLatency() {

	// close and cleanup all files
	for (auto& it : files) {
		it.second->close();
		delete it.second;
	}

}


void ExpPlotLatency::onNewTcpConnection(HTTPAnalyzer& analyzer, HTTPDecoder& dec) {

	(void) analyzer;

	// a new TCP-connection has been opened and a HTTPDecoder was created
	// register myself within this decoder to get latency information
	dec.addListener(*this);

}

void ExpPlotLatency::open(const std::string& contentType) {

	debug(DBG_EXP_LATENCY, DBG_LVL_INFO, "creating file for content-type: " << contentType);

	// we need to replace '/' within the content-type
	std::string filename = contentType + ".dat";
	strReplaceChar(filename, '/', '-');

	// create folders and get filename
	File folder = File(Exporter::folder, "latency");
	folder.mkdirs();
	File file = File(folder, filename);

	files[contentType] = new std::ofstream();
	files[contentType]->open(file.getAbsolutePath().c_str());
	*files[contentType] << "# latency for " << contentType << std::endl;

}

void ExpPlotLatency::append(const std::string& contentType, uint32_t latency) {
	if (files.find(contentType) == files.end()) {open(contentType);}
	*files[contentType] << latency << std::endl;
}


void ExpPlotLatency::onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& req) {
	(void) dec;
	(void) req;
}

void ExpPlotLatency::onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& req, const HTTPHeader& resp, uint32_t latency) {
	(void) dec;
	(void) req;
	append(resp.getContentType(), latency);
}

void ExpPlotLatency::onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) {
	(void) dec;
	(void) reqHeader;
	(void) data;
	(void) length;
}

void ExpPlotLatency::onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader,  const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) {
	(void) dec;
	(void) reqHeader;
	(void) respHeader;
	(void) data;
	(void) length;
}

void ExpPlotLatency::onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) {
	(void) dec;
	(void) header;
}

void ExpPlotLatency::onHttpClose(const HTTPDecoder& dec) {
	(void) dec;
}

void ExpPlotLatency::visit(NetSource& src) {

	// add to NetSource-Listeners to get latency information
	src.addListener(this);

}
