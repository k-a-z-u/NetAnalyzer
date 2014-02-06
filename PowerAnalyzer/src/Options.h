#ifndef PA_OPTIONS_H
#define PA_OPTIONS_H

#include "common/Utils.h"

#include <string>
#include <cstdint>

namespace PowerAnalyzer {

enum class OperationMode {
	Client,
	Server,
	Invalid
};

class Options {
public:
	Options();
	~Options();

	static void printUsage(const std::string& progname);

	bool parseCmdline(int argc, char* argv[]);
//	bool parseCfg(const std::string& file);

	OperationMode getMode() const;
	std::string getDataDir() const;
	std::string getServer() const;
	uint16_t getPort() const;
	DataCompression getCompression() const;
	uint32_t getChunkSize() const;
	int getReplaySpeed() const;
	int getReplayCount() const;
	bool verifyData() const;
	bool useDirectIO() const;
	int getCPUCore() const;

private:
	bool allOptionsValid() const;

	OperationMode modeFromArg(const char* arg) const;
	uint16_t portFromArg(const char* arg) const;
	DataCompression comprFromArg(const char* arg) const;
	uint32_t chunkSizeFromArg(const char* arg) const;
	int replaySpeedFromArg(const char* arg) const;
	int replayCountFromArg(const char* arg) const;
	int dbgLevelFromArg(const char* arg) const;

	OperationMode mode = OperationMode::Invalid;
	std::string server;
	std::string dataDir;
	uint16_t port = 8468; // default port
	DataCompression dataCompr = DataCompression::None;
	uint32_t chunkSize = 1024; // default chunk size
	int replaySpeed = 1;
	int replayCount = 1;
	bool verify = false;
	bool directio = true;
	int cpuCore = 0; // by default bind to core 0; set this to -1 to not bind
};

} // namespace PowerAnalyzer

#endif // PA_OPTIONS_H
