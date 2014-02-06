#include "ClientStatsExporter.h"
#include "ClientStats.h"
#include "../common/Utils.h"
#include "../../../NetAnalyzer/src/Helper.h"

#include <iostream>

namespace PowerAnalyzer {

ClientStatsExporter::ClientStatsExporter(const Options& op, const ClientStats& stats)
: options(op), stats(stats), os(getExportFilename()) {
	if (!os) {
		throw "Unable to create file to export client stats.";
	}

	// export every minute
	setIntervalMs(1000 * 60);

	// the options go first
	printOptions(os);

	// print the stats header
	const auto header = "# timestamp\tdatTimestamp\tbytesComr\tbytesUncompr\tdecomprTimeMs\t/proc/stat\n";
	os << header;

	if (dbgLevel >= DBG_LVL_WARN) {
		printOptions(std::cout);
		std::cout << header;
	}
}

ClientStatsExporter::~ClientStatsExporter() {

}

void ClientStatsExporter::execTasks() {

	if (!stats.lastFileTimestamp) {
		// nothing done - nothing to export yet
		return;
	}

	printCurrentStats(os);

	if (dbgLevel >= DBG_LVL_WARN) {
		printCurrentStats(std::cout);
	}

}

std::string ClientStatsExporter::getExportFilename() const {

	return std::string("clientExports_") + std::to_string(getTimeStampMS()) + ".dat";
}

void ClientStatsExporter::printOptions(std::ostream& os) {
	// print the options header
	os << "# mode\tserver\tport\tdatDir\tcompression\tchecksum\tchunksize\treplayspeed\treplaycount\tverbose\n";

	// print options values
	os << "# client" << "\t"
	   << options.getServer() << "\t"
	   << options.getPort() << "\t"
	   << options.getDataDir() << "\t"
	   << getCompressionName(options.getCompression()) << "\t"
	   << (int)options.verifyData() << "\t"
	   << options.getChunkSize() << "\t"
	   << options.getReplaySpeed() << "\t"
	   << options.getReplayCount() << "\t"
	   << dbgLevel << "\n";
}

void ClientStatsExporter::printCurrentStats(std::ostream& os) {
	os << getTimeStampMS() << "\t" // timestamp
	   << stats.lastFileTimestamp << "\t" // timestamp from .dat file
	   << stats.payloadReceived << "\t" // number of compressed bytes received from server
	   << stats.bytesDecompressedPayload << "\t" // number of uncompressed bytes decompressed from server payload
	   << (stats.decompressionTimeUs / 1000) << "\t" // decompression time in milliseconds
	   << getCPUStatString(stats.cpuCore) << std::endl;
}

} // namespace PowerAnalyzer
