/*
 * Analyzer.cpp
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#include "Analyzer.h"
#include "../Debug.h"

#include "HTTPAnalyzer.h"
#include "HTTPDataSource.h"

#include "../compression/Compressor.h"
#include "../export/Exporter.h"

#include "../gui/GUI.h"

#define DBG_ANALYZER	"Analyzer"


Analyzer::Analyzer() : gui(nullptr), srcHttp(nullptr) {
	// TODO Auto-generated constructor stub

}

Analyzer::~Analyzer() {
	// TODO Auto-generated destructor stub
}

void Analyzer::addCompressorDecompressorPrototype(Compressor* const compressor, Decompressor* const decompressor) {
	CompressorDecompressor cd(compressor, decompressor);
	compressors.push_back(cd);
}

void Analyzer::setGUI(GUI* newGUI) {
	gui = newGUI;
	gui->setModel(summary);
	gui->update();
}

void Analyzer::addExporter(Exporter& exporter) {

	// provide access to the model for the exporter
	exporter.setModel(summary);

	// let the HTTPSource visit the exporter
	// the exporter may then register itself for callbacks within the HTTPSource
	if (srcHttp != nullptr) {
		srcHttp->accept(exporter);
	}

}

HTTPAnalyzer* Analyzer::onHttpConnection() {

	debug(DBG_ANALYZER, DBG_LVL_INFO, "creating new HTTPAnalyzer");

	// create a new HTTPAnalyzer and set the summary
	// the HTTPAnalyzer will store it's facts to
	HTTPAnalyzer* httpAnalyzer = new HTTPAnalyzer(summary);

	// create a new compressor from each prototype
	// and attach it to the HTTPAnalyzer
	for (const auto& pair : compressors) {
		Compressor* c = pair.comp->create();
		Decompressor* d = (pair.decomp) ? (pair.decomp->create()) : (nullptr);
		httpAnalyzer->addCompressorDecompressor(c, d);
	}

	// finally, return it
	return httpAnalyzer;

}

void Analyzer::setHTTPDataSource(HTTPDataSource* src) {
	src->accept(*this);
	srcHttp = src;
}

Summary& Analyzer::getSummary() const {
	return (Summary&) summary;
}
