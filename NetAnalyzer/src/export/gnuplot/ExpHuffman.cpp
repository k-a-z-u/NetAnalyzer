/*
 * ExpHuffman.cpp
 *
 *  Created on: Apr 21, 2013
 *      Author: kazu
 */

#include "ExpHuffman.h"
#include "../../analyzer/Summary.h"
#include "../../misc/File.h"
#include "../../misc/MyString.h"
#include "../../Debug.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#define DBG_EXP_HUFFMAN		"expHuff"

ExpHuffman::ExpHuffman(PeriodicExporterThread* thread) : PeriodicExporter(thread) {
	debug(DBG_EXP_HUFFMAN, DBG_LVL_INFO, "creating huffman-exporter");
}

ExpHuffman::~ExpHuffman() {

	// close and cleanup all files
	for (auto& it : contentTypes) {
		it.second->close();
		delete it.second;
	}

}

void ExpHuffman::addContentType(const std::string& type) {

	debug(DBG_EXP_HUFFMAN, DBG_LVL_INFO, "adding content-type: " << type);

	// we need to replace '/' within the content-type
	std::string filename = type;
	strReplaceChar(filename, '/', '-');

	// get filename and create directories
	File folder = File(Exporter::folder, "huffman");
	folder.mkdirs();
	File file = File(folder, filename + ".dat");

	// create stream
	std::ofstream* stream = new std::ofstream();
	stream->open(file.getAbsolutePath().c_str());
	contentTypes[type] = stream;

}

void ExpHuffman::exportMe() {

	// process each selected content-type
	for (const auto& it : contentTypes) {

		// check if content-type exists
		const auto& type = sum->HTTP.byContentType.find(it.first);
		if (type == sum->HTTP.byContentType.end()) {continue;}

		// export current content-type
		const uint32_t* values = type->second.huffman.getCounterValues();
		for (uint32_t i = 0; i <= 0xFF; ++i) {
			*(it.second) << values[i] << "\t";
		}
		*(it.second) << std::endl;

	}

}

