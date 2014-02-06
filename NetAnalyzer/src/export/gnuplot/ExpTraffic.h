/*
 * ExpTraffic.h
 *
 *  Created on: Apr 29, 2013
 *      Author: kazu
 */

#ifndef EXPTRAFFIC_H_
#define EXPTRAFFIC_H_

#include "../PeriodicExporter.h"
#include "../../misc/File.h"

#include <fstream>
#include <map>

/** per content-type */
struct ExpTraf {

	/** the file to write to */
	std::ofstream* stream = nullptr;

	/** execute some steps only on first run */
	bool firstRun = true;

	/** store old traffic values here (for relative traffic values) */
	//std::map<std::string, uint64_t> oldTrafficValues;

	/** store old latency values here (for relative latency values) */
	//std::map<std::string, uint64_t> oldLatencyValues;

	/** all content types to write to this stream */
	std::vector<std::string> contentTypes;

};

/**
 * exports data to draw a chart for traffic analysis.
 * the export provides values for:
 * 	uncompressed-traffic, huffman-traffic, compressors-traffics
 */
class ExpTraffic : public PeriodicExporter {

public:

	/** ctor */
	ExpTraffic(PeriodicExporterThread* thread);

	/** dtor */
	virtual ~ExpTraffic();

	/** add a new content-type to export */
	void addContentType(const std::string& contentType, const std::string& bin);

private:

	void exportMe() override;

	/** the folder for all traffic files */
	File folder;

	/** all output files (to combine several content-types to one file) */
	std::map<std::string, ExpTraf> files;

};

#endif /* EXPTRAFFIC_H_ */
