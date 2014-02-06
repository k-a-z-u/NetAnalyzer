#include "ServerStats.h"
#include "../../../NetAnalyzer/src/Helper.h"

#include <fstream>

namespace PowerAnalyzer {

void ServerStats::dump(std::ostream& os) const {

	// TODO: write server start date/time.

	const auto currentTime = getTimeStampMS();
	os << "Runtime: " << ((currentTime - startTime) / 1000) << "s" << "\n"
	   << "CPU core: " << cpuCore << "\n"
	   << "Data directory: " << dataDir << "\n"
	   << "Clients served: " << clientsServed << "\n"
	   << "Bytes read: " << bytesRead << "\n"
	   << "Files sent: " << filesSent << "\n"
	   << "Bytes sent: " << bytesSent << "\n"
	   << "Payload sent: " << payloadSent << "\n"
	   << "Bytes received: " << bytesReceived << "\n"
	   << "Compression time: " << (compressionTimeUs / 1000) << "ms\n"
	   << "Checksum time: " << (checksumTimeUs / 1000) << "ms"
	   << std::endl;
}

void ServerStats::dump(const std::string& file) const {
	std::ofstream os(file);
	if (!os) {
		throw "Unable to create file for server stats.";
	}

	dump(os);
}

} // namespace PowerAnalyzer
