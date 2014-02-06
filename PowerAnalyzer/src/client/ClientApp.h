#ifndef PA_CLIENT_APP_H
#define PA_CLIENT_APP_H

#include "../Options.h"
#include "ClientStats.h"
#include "PayloadMetaDirectory.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class Decompressor;

namespace PowerAnalyzer {

class ClientSocket;

class ClientApp {
public:
	explicit ClientApp(const Options& op);
	~ClientApp();

	void run();
	void stop();

private:
	RequestMetaMap readClientRequests() const;
	void waitForStart() const;
	void runRequestLoop(uint64_t clientStartTime, uint64_t firstRequestTime,
						const RequestMetaMap& requests);
	int waitTillRequest(uint64_t clientStartTime, uint64_t firstRequestTime,
						uint64_t currentRequestTime) const;
	int calcRepeatWaitTime(RequestMetaMap::const_iterator& requestIter,
						   const RequestMetaMap::const_iterator& requestsEnd,
						   const int msTooLate);
	void handleRepeatedRequest(const RequestMetaMap::const_iterator& requestIter,
							   int waitTimeRepeat);
	int handleRequest(const std::vector<std::string>& filenames);
	void connect(ClientSocket& client);
	void requestFile(ClientSocket& client, const std::string& filename);
	void receiveFile(ClientSocket& client);
	void verifyChunk(const std::vector<char>& chunk, uint32_t checksum) const;
	void decompressChunk(const std::vector<char>& compressed) const;

	const Options options;

	mutable ClientStats stats;

	std::map<DataCompression, std::unique_ptr<Decompressor>> decompressors;
	Decompressor* activeDecompressor = nullptr;
};

} // namespace PowerAnalyzer

#endif // PA_CLIENT_APP_H
