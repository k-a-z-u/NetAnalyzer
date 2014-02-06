#ifndef PA_SERVER_STATS_H
#define PA_SERVER_STATS_H

#include <iosfwd>
#include <string>

namespace PowerAnalyzer {

struct ServerStats {
	std::string outputFile;

	std::string dataDir;
	int cpuCore = -1;
	unsigned long long startTime = 0;
	std::string lastFileTimestamp;
	unsigned int clientsServed = 0;
	unsigned int filesSent = 0;
	unsigned long long bytesRead = 0;
	unsigned long long bytesSent = 0;
	unsigned long long payloadSent = 0;
	unsigned long long bytesReceived = 0;
	unsigned long long compressionTimeUs = 0;
	unsigned long long checksumTimeUs = 0;

	void dump(std::ostream& os) const;
	void dump(const std::string& file) const;
};

} // namespace PowerAnalyzer

#endif // PA_SERVER_STATS_H
