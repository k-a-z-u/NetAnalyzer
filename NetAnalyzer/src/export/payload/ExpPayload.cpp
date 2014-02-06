/*
 * ExpPayload.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#include "ExpPayload.h"

#include "../../Debug.h"
#include "../../Helper.h"

#include "../../misc/MyString.h"
#include "../../misc/File.h"

#include "../../http/HTTPDecoder.h"
#include "../../http/HTTPHeader.h"

#include "../../analyzer/source/NetSource.h"

#include <fstream>

#define DBG_EXP_PAYLOAD		"expPay"


ExpPayload::ExpPayload() {

	debug(DBG_EXP_PAYLOAD, DBG_LVL_INFO, "creating payload-exporter");

	// create storage folder
	folder = File(Exporter::folder, "payload");
	folder.mkdirs();

}

ExpPayload::~ExpPayload() {

	// perform cleanup
	for (auto& it : contentTypes) {
		it.second.stream->close();
		delete it.second.stream;
	}
	for (auto& it : openFiles) {
		it.second->close();
		delete it.second;
	}

}

void ExpPayload::addContentType(const std::string& contentType, const std::string& fileExt) {

	debug(DBG_EXP_PAYLOAD, DBG_LVL_INFO, "adding content-type " << contentType << " with file-extension " << fileExt);

	// store file-extension for this content type
	contentTypes[contentType].fileExt = fileExt;

	// we need to replace '/' within the content-type
	std::string ct = contentType;
	strReplaceChar(ct, '/', '-');

	// where to store global stats file
	File statFile = File(folder, ct + ".dat");

	// where to store payloads
	contentTypes[contentType].folder = File(folder, ct);
	contentTypes[contentType].folder.mkdirs();

	// open stream for details-file
	contentTypes[contentType].stream = new std::ofstream(statFile.getAbsolutePath().c_str());
	*(contentTypes[contentType]).stream << "#timestamp\trelTS\tlatency\tfilename" << std::endl;

}

void ExpPayload::onNewTcpConnection(HTTPAnalyzer& analyzer, HTTPDecoder& dec) {
	(void) analyzer;
	dec.addListener(*this);
}


void ExpPayload::onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& reqHeader) {
	(void) dec;
	(void) reqHeader;
}

void ExpPayload::onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, uint32_t latency) {

	(void) reqHeader;

	// check whether this content-type is to be exported
	if (contentTypes.find(respHeader.getContentType()) == contentTypes.end()) {return;}

	// check whether we expect some payload after this response
	if (reqHeader.getMethod() == HTTP_METHOD_HEAD) {return;}

	// cleanup
	if (openFiles.find(&dec) != openFiles.end()) {
		openFiles[&dec]->close();
		delete openFiles[&dec];
		openFiles.erase(&dec);
	}

	// get details of this content-type
	const ExpPayContentType& det = contentTypes[respHeader.getContentType()];

	uint64_t absTS = respHeader.getTimestamp();
	uint64_t relTS = getUptimeMS();

	// create file-name
	std::stringstream ss;
	ss << absTS << "_" << rand() << "." << det.fileExt;

	// get storage file-name
	File file = File(det.folder, ss.str());

	// add entry to stats-file
	*(det.stream) << absTS << "\t" << relTS << "\t" << latency << "\t" << ss.str() << std::endl;

	// create/open a new output stream
	std::ofstream* stream = new std::ofstream();
	stream->open(file.getAbsolutePath().c_str());
	openFiles[&dec] = stream;

}

void ExpPayload::onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) {
	(void) dec;
	(void) reqHeader;
	(void) data;
	(void) length;
}

void ExpPayload::onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) {

	(void) reqHeader;
	(void) respHeader;

	// sanity check
	if (openFiles.find(&dec) == openFiles.end()) {return;}

	// append
	openFiles[&dec]->write( (char*) data, length);

}

void ExpPayload::onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) {

	(void) header;

	// cleanup
	if (openFiles.find(&dec) != openFiles.end()) {
		openFiles[&dec]->close();
		delete openFiles[&dec];
		openFiles.erase(&dec);
	}

}

void ExpPayload::onHttpClose(const HTTPDecoder& dec) {

	// cleanup
	if (openFiles.find(&dec) != openFiles.end()) {
		openFiles[&dec]->close();
		delete openFiles[&dec];
		openFiles.erase(&dec);
	}

}

void ExpPayload::visit(NetSource& src) {

	// add to NetSource-Listeners to get latency information
	src.addListener(this);

}
