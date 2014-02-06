#include "ClientStats.h"
#include "../../../NetAnalyzer/src/Helper.h"

#include <fstream>

namespace PowerAnalyzer {

void ClientStats::dump(std::ostream& os) const {

	// TODO: write client start date/time.

	const auto currentTime = getTimeStampMS();
	os << "Runtime: " << ((currentTime - startTime) / 1000) << "s" << "\n"
	   << "Potential runtime: " << ((potentialEndTime - startTime) / 1000 / 60) << "min" << "\n"
	   << "CPU core: " << cpuCore << "\n"
	   << "Replay speed: " << replaySpeed << "\n"
	   << "Replays planned: " << replaysPlanned << "\n"
	   << "Replays done: " << replaysDone << "\n"
	   << "Meta directory: " << metaDir << "\n"
	   << "Content types: ";

	for (const auto& ct : contentTypes) {
		os << ct << " ";
	}

	os << "\n"
	   << "Files available: " << filesAvailable << "\n"
	   << "Unique timestamps: " << uniqueTimestamps << "\n"
	   << "Sleep time: " << (sleepTime / 1000) << "s" << "\n"
	   << "Too late time: " << (tooLateTime / 1000) << "s" << "\n"
	   << "Connections: " << connections << "\n"
	   << "Files requested: " << filesRequested << "\n"
	   << "Bytes sent: " << bytesSent << "\n"
	   << "Bytes received: " << bytesReceived << "\n"
	   << "Payload received: " << payloadReceived << "\n"
	   << "Payload decompressed: " << bytesDecompressedPayload << "\n"
	   << "Decompression time: " << (decompressionTimeUs / 1000) << "ms\n"
	   << "Checksum time: " << (checksumTimeUs / 1000) << "ms"
	   << std::endl;
}

void ClientStats::dump(const std::string& file) const {
	std::ofstream os(file);
	if (!os) {
		throw "Unable to create file for server stats.";
	}

	dump(os);
}

} // namespace PowerAnalyzer
