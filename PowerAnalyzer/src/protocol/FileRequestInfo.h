#ifndef PA_FILE_REQUEST_INFO_H
#define PA_FILE_REQUEST_INFO_H

#include "../common/Utils.h"

#include <string>
#include <vector>
#include <cstdint>

namespace PowerAnalyzer {

static const size_t FILE_REQUEST_CHECKSUM = 0x01;

struct FileRequestInfo {

	static const size_t FILENAME_LEN = 64;
	static const size_t PROTOCOL_SIZE = FILENAME_LEN + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t);

	static FileRequestInfo fromProtocolBuffer(const std::vector<char>& buf);
	static std::vector<char> toProtocolBuffer(const std::string& filename, DataCompression compression, uint32_t chunkSize, bool checksum);

	std::string filename;
	DataCompression compression;
	uint32_t chunkSize = 0;
	uint8_t flags = 0;

private:
	FileRequestInfo();

};

} // namespace PowerAnalyzer

#endif // PA_FILE_REQUEST_INFO_H
