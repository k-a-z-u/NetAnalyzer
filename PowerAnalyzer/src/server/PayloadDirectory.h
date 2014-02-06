#ifndef PA_PAYLOAD_DIR_H
#define PA_PAYLOAD_DIR_H

#include "../common/Utils.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class Compressor;

namespace PowerAnalyzer {

class ServerStats;

class PayloadDirectory {
public:
	PayloadDirectory(const std::string& dir);
	~PayloadDirectory();

	void openFile(const std::string& filepath, DataCompression compression, bool directio);

	/* trying to read from a file that reached EOF (-> fileDone()) is undefined */
	std::vector<char> readFile(uint32_t chunkSize, bool calcChecksum, uint32_t& checksum, ServerStats& stats);

	/* EOF reached or not */
	bool fileDone() const;

	std::vector<char> getRemainingData(bool calcChecksum, uint32_t& checksum, ServerStats& stats);

private:
	uint32_t calcChecksum(const std::vector<char>& data, ServerStats& stats);
	std::vector<char> compressData(const std::vector<char>& data) const;

	const std::string dataDir;

//	std::ifstream inputFileStream;
	int fileDescriptor = -1;
	bool fileDescriptorEOF = false;
	long alignmentNeeded = 0;

	std::map<DataCompression, std::unique_ptr<Compressor>> compressors;
	Compressor* activeCompressor = nullptr;
};

} // namespace PowerAnalyzer

#endif // PA_PAYLOAD_DIR_H
