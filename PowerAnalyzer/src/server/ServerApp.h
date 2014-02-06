#ifndef PA_SERVER_APP_H
#define PA_SERVER_APP_H

#include "../Options.h"
#include "ServerStats.h"

#include <string>
#include <vector>

namespace PowerAnalyzer {

class Options;
class Socket;
class ServerSocket;
class PayloadDirectory;
class FileRequestInfo;

class ServerApp {
public:
	explicit ServerApp(const Options& op);
	~ServerApp();

	void run();
	void stop();

private:
	void start(ServerSocket& server);
	void serveClient(Socket& client, PayloadDirectory& dir);
	FileRequestInfo recvRequestInfo(Socket& client);
	void sendFile(const FileRequestInfo& info, PayloadDirectory& dir, Socket& client);
	size_t sendFileChunk(const std::vector<char>& chunk, uint32_t checksum, Socket& client);
	std::string timestampFromPath(const std::string& filename) const;

	const Options options;

	mutable ServerStats stats;
};

} // namespace PowerAnalyzer

#endif // PA_SERVER_APP_H
