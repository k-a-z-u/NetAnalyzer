/*
 * ExpTraffic.cpp
 *
 *  Created on: Apr 29, 2013
 *      Author: kazu
 */

#include "ExpTraffic.h"

#include "../../misc/MyString.h"
#include "../../analyzer/Summary.h"


ExpTraffic::ExpTraffic(PeriodicExporterThread* thread) : PeriodicExporter(thread) {

	// create folder for all traffic files
	folder = File(Exporter::folder, "traffic");
	folder.mkdirs();

}

ExpTraffic::~ExpTraffic() {

	// cleanup (close all files)
	for (auto file : files) {
		file.second.stream->close();
		delete file.second.stream;
	}

}

void ExpTraffic::addContentType(const std::string& contentType, const std::string& bin) {

	// check whether file for this bin already exists
	if (files.find(bin) == files.end()) {

		// create new file for this bin
		File file = File(folder, bin + ".dat");
		std::ofstream* stream = new std::ofstream();
		stream->open(file.getAbsolutePath().c_str());

		// store
		files[bin].stream = stream;

		// write file header (1st part)
		*(stream) << "# traffic values exported every " << exportIntervalSec << " seconds." << std::endl;
		*(stream) << "# the values are cumulative." << std::endl;
		*(stream) << "# every second column contains the used latency in milliseconds." << std::endl;
		*(stream) << "# columns: timestamp, uncompressed, ";

	}

	// now attach the content type
	ExpTraf* entry = &files[bin];
	entry->contentTypes.push_back(contentType);

}

void ExpTraffic::exportMe() {

	// export stats for each bin (file)
	for (auto& file : files) {

		uint32_t numCompressors = 0;

		uint64_t sumUncompressed = 0;
		uint64_t sumUncompressedLatency = 0;

		uint64_t sumCompressed[32] = {0};
		uint64_t sumCompressedLatency[32] = {0};

		// sum-up all requested content types for this file
		for (auto& ct : file.second.contentTypes) {

			// try to get the stats for this content-type (if yet available)
			const auto type = sum->HTTP.byContentType.find(ct);
			if (type == sum->HTTP.byContentType.end()) {continue;}

			// TODO: better place for this???
			// write file-header on 1st run
			if (file.second.firstRun) {
				file.second.firstRun = false;

				// append all available compressors
				for (auto& comp : type->second.response.comprStats) {
					*(file.second.stream) << comp.first << ", ";
				}
				*(file.second.stream) << std::endl;

			}

			sumUncompressed += type->second.response.sizePayload;
			sumUncompressedLatency += type->second.response.latency.getSum();

			// now export stats for each compressor
			uint32_t compressorIDX = 0;
			for (auto& comp : type->second.response.comprStats) {

				sumCompressed[compressorIDX] += comp.second.compression.compressedPayloadSize;
				sumCompressedLatency[compressorIDX] += comp.second.compression.compressedPayloadUs;
				++compressorIDX;

			}

			// store the number of found compressors
			numCompressors = compressorIDX;

		}


		// all exported values are cumulative

		// the timestamp of the export
		*(file.second.stream) << getTimeStampMS() << "\t";

		// export uncompressed payload size
		*(file.second.stream) << sumUncompressed << "\t";

		// export uncompressed latency
		*(file.second.stream) << sumUncompressedLatency << "\t";

		// now export all compressor stats
		for (uint32_t idx = 0; idx < numCompressors; ++idx) {
			*(file.second.stream) << sumCompressed[idx] << "\t";
			*(file.second.stream) << (sumCompressedLatency[idx] / 1000) << "\t";
		}

		// done
		*(file.second.stream) << std::endl;


	}

}
