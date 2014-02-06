#ifndef PA_CLIENT_STATS_H
#define PA_CLIENT_STATS_H

#include <string>
#include <vector>

namespace PowerAnalyzer {

struct ClientStats {
	std::string outputFile;

	std::string metaDir;
	std::vector<std::string> contentTypes;
	int replaySpeed = 0;
	int replaysPlanned = 0;
	int replaysDone = 0;
	int cpuCore = -1;
	unsigned long long startTime = 0;
	unsigned long long lastFileTimestamp = 0;
	unsigned long long potentialEndTime = 0;
	unsigned int filesAvailable = 0;
	unsigned int uniqueTimestamps = 0;
	unsigned long long sleepTime = 0;
	unsigned int tooLateTime = 0;
	unsigned int connections = 0;
	unsigned int filesRequested = 0;
	unsigned long long bytesSent = 0;
	unsigned long long bytesReceived = 0;
	unsigned long long payloadReceived = 0;
	unsigned long long bytesDecompressedPayload = 0;
	unsigned long long decompressionTimeUs = 0;
	unsigned long long checksumTimeUs = 0;

	void dump(std::ostream& os) const;
	void dump(const std::string& file) const;
};

} // namespace PowerAnalyzer

#endif // PA_CLIENT_STATS_H
