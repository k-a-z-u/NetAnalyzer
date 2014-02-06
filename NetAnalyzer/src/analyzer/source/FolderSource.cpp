/*
 * FolderSource.cpp
 *
 *  Created on: May 15, 2013
 *      Author: kazu
 */

#include "FolderSource.h"
#include "../../Debug.h"
#include "../../http/HTTPDecoder.h"
#include "../../http/HTTPHeader.h"

#include "../Analyzer.h"
#include "../HTTPAnalyzer.h"

#include <sys/stat.h>

#include <fstream>

#define DBG_FOLDER_SOURCE		"FoldSrc"


FolderSource::FolderSource(File& folder, uint32_t chunkSize) :
	analyzer(nullptr), folder(folder), running(false), chunkSize(chunkSize) {

	buffer = (uint8_t*) malloc(chunkSize);

}

FolderSource::~FolderSource() {
	if (buffer) {free(buffer); buffer = nullptr;}
}

void FolderSource::sendFiles() {

	// dummy http decoder
	HTTPDecoder dummyDec;

	// start with the root folder and walk subfolders recursively
	sendFolder(folder, dummyDec);

}

void FolderSource::sendFolder(File folder, HTTPDecoder& dec) {

	// get all files in this folder
	std::vector<File> files;
	folder.listFiles(files);

	// struct for filesize-status
	struct stat fileStats;

	// process each file
	for (const auto file : files) {

		// if this file is another folder -> recursion
		if (file.isFolder()) {

			// send this subfolder
			sendFolder(file, dec);

		} else {

			// check file-size to skip 0-byte files
			stat(file.getAbsolutePath().c_str(), &fileStats);
			if (fileStats.st_size == 0) {continue;}

			// send one file
			sendFile(file, dec);

			// check if canceled?
			if (!running) {break;}

		}

	}

}

void FolderSource::sendFile(File file, HTTPDecoder& dec) {

	debug(DBG_FOLDER_SOURCE, DBG_LVL_INFO, "sending file: " << file.getAbsolutePath());

	// get new HTTP analyzer to send the file to
	HTTPAnalyzer* http = analyzer->onHttpConnection();

	// send dummy request
	HTTPHeader hReq;
	hReq.setMethod(HTTP_METHOD_GET);
	http->onHttpRequest(dec, hReq);

	// content-type is the folder-name realtive to the source-folder
	uint32_t rootLength = folder.getAbsolutePath().length()+1;
	std::string contentType = file.getParent().getAbsolutePath();
	contentType = (contentType.length() > rootLength) ? (contentType.substr(rootLength)) : ("root");


	// send dummy response
	HTTPHeader hResp;
	hResp.setMethod(HTTP_METHOD_RESPONSE);
	hResp.setCode(200);
	hResp.set("content-type", contentType);

	http->onHttpResponse(dec, hReq, hResp, 0);

	// read the file
	std::ifstream stream;
	stream.open(file.getAbsolutePath().c_str(), std::ios::in);
	if (stream.is_open()) {

		while (true) {

			// read one block;
			stream.read( (char*) buffer, chunkSize );
			size_t numRead = stream.gcount();

			// send to analyzer
			http->onHttpResponseTraffic(dec, hReq, hResp, buffer, numRead);

			// EOF?
			if (stream.eof()) {break;}

		}

	}

	// finish (flush compressors)
	http->onHttpDone(dec, hResp);

	// close (this will delete the http analyzer)
	http->onHttpClose(dec);

}

void FolderSource::accept(Analyzer& analyzer) {
	this->analyzer = &analyzer;
}

void FolderSource::accept(Exporter& exp) {
	(void) exp;
	// the folder source has no usable data for exporters
	// -> nothing to do here
}

void FolderSource::start() {
	running = true;
	sendFiles();
}

void FolderSource::stop() {
	running = false;
}
