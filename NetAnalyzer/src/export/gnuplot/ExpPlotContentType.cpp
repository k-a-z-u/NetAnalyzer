/*
 * ExpPlotContentType.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#include "ExpPlotContentType.h"
#include "../../Helper.h"
#include "../../analyzer/Summary.h"
#include "../../Debug.h"
#include "../../misc/File.h"

#include <cstring>

#define DBG_EXP_CONTENTTYPE		"expConT"



ExpPlotContentType::ExpPlotContentType(PeriodicExporterThread* thread) : PeriodicExporter(thread), first(true), numColumns(1) {

	debug(DBG_EXP_CONTENTTYPE, DBG_LVL_INFO, "creating content-type-exporter");

	// get export-filename
	File folder = File(Exporter::folder, "contentTypes");
	folder.mkdirs();
	File file = File(folder, "all.dat");

	stream.open(file.getAbsolutePath().c_str());
	stream << "# cumulative traffic amount per content-type in bytes since program-start" << std::endl;
	stream << "# every second column contains a copy of the previous value, that's only for convenience" << std::endl;
	stream << "# purpose to allow relative and absolute calculations within gnuplot." << std::endl;

}

ExpPlotContentType::~ExpPlotContentType() {
	stream.close();
}


void ExpPlotContentType::addContentType(const std::string& contentType, uint32_t column) {

	debug(DBG_EXP_CONTENTTYPE, DBG_LVL_INFO, "adding content-type " << contentType << " to column " << column);

	assertLowerCase(contentType, "plot.addContentType() must use lower-case content-types!");

	contentTypes[contentType] = column;

	numColumns = column + 1;

	if (numColumns > PLOT_MAX_COLUMS) {
		error("PLOT", "too many columns. max: " << PLOT_MAX_COLUMS);
	}

}

void ExpPlotContentType::exportMe() {

	// first export? dump column headers
	if (first) {

		first = false;
		std::string tmpArr[PLOT_MAX_COLUMS+2];

		// store column names in a temporal array
		tmpArr[0] = "sum of values";
		for (const auto& it : contentTypes) {tmpArr[1+it.second] += it.first + "; ";}
		tmpArr[numColumns+1] = "other";

		// write the header
		stream << "#0:\ttimestamp" << std::endl;
		for (uint32_t i = 0; i < numColumns+2; ++i) {
			stream << "#" << (i+1) << ":\t" << tmpArr[i] << std::endl;
		}

	}


	/** store values of current call */
	uint64_t curValues[PLOT_MAX_COLUMS+2] = {0};

	// map each content-type to a column index until max-entries are reached
	for (const auto& it : sum->HTTP.byContentType) {

		// arr[0]: traffic sum
		curValues[0] += it.second.response.sizePayload;

		// arr[last]: "other"
		// content-type not selected for export.
		// -> add to "other" traffic (last column of array)
		if (contentTypes.find(it.first) == contentTypes.end()) {
			curValues[1+numColumns] += it.second.response.sizePayload;
			continue;
		}

		// arr[1-(last-1)]: requested content-types
		// insert into temporal array
		// it's possible to map several content-types to the same column -> "+="
		uint32_t index = contentTypes[it.first];
		curValues[1+index] += it.second.response.sizePayload;

	}

	// add timestamp
	stream << (getTimeStampMS()/1000) << "\t";

	// finally add each column to the result
	// (+1 for "other", +1 for "sum")
	for (uint32_t i = 0; i < numColumns + 2; ++i) {
		stream << curValues[i] << "\t";
	}
	stream << std::endl;

}
