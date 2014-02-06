#include "ServerStatsExporter.h"
#include "ServerStats.h"
#include "../common/Utils.h"
#include "../../../NetAnalyzer/src/Helper.h"
#include "../../../NetAnalyzer/src/Debug.h"

#include <iostream>

namespace PowerAnalyzer {

ServerStatsExporter::ServerStatsExporter(const Options& op, const ServerStats& stats)
: options(op), stats(stats), os(getExportFilename()) {
	if (!os) {
		throw "Unable to create file to export server stats.";
	}

	// export every minute
	setIntervalMs(1000 * 60);

	// the options go first
	printOptions(os);

	// print the stats header
	const auto header = "# timestamp\tdatTimestamp\tbytesComr\tbytesUncompr\tcomprTimeMs\t/proc/stat\n";
	os << header;

	if (dbgLevel >= DBG_LVL_WARN) {
		printOptions(std::cout);
		std::cout << header;
	}
}

ServerStatsExporter::~ServerStatsExporter() {

}

void ServerStatsExporter::execTasks() {

	if (stats.lastFileTimestamp.empty()) {
		// nothing done - nothing to export yet
		return;
	}

	printCurrentStats(os);

	if (dbgLevel >= DBG_LVL_WARN) {
		printCurrentStats(std::cout);
	}

}

std::string ServerStatsExporter::getExportFilename() const {

	return std::string("serverExports_") + std::to_string(getTimeStampMS()) + ".dat";
}

void ServerStatsExporter::printOptions(std::ostream& os) {
	// print the options header
	os << "# mode\tport\tpayloadDir\tverbose\n";

	// print options values
	os << "# server" << "\t"
	   << options.getPort() << "\t"
	   << options.getDataDir() << "\t"
	   << dbgLevel << "\n";
}

void ServerStatsExporter::printCurrentStats(std::ostream& os) {
	os << getTimeStampMS() << "\t" // timestamp
	   << stats.lastFileTimestamp << "\t" // timestamp from .dat file
	   << stats.payloadSent << "\t" // number of bytes compressed and sent to clients
	   << stats.bytesRead << "\t" // number of uncompressed bytes read from file
	   << (stats.compressionTimeUs / 1000) << "\t" // compression time in milliseconds
	   << getCPUStatString(stats.cpuCore) << std::endl;
}

} // namespace PowerAnalyzer
