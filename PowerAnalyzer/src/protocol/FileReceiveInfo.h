#ifndef PA_FILE_RECEIVE_INFO_H
#define PA_FILE_RECEIVE_INFO_H

#include "../common/Utils.h"

#include <vector>
#include <cstdint>

namespace PowerAnalyzer {

struct FileReceiveInfo {

	static const size_t PROTOCOL_SIZE = sizeof(uint32_t) + sizeof(uint32_t);

	static FileReceiveInfo fromProtocolBuffer(const std::vector<char>& buf);
	static std::vector<char> toProtocolBuffer(uint32_t chunkSize);
	static std::vector<char> toProtocolBuffer(uint32_t chunkSize, uint32_t checksum);

	uint32_t chunkSize;
	uint32_t checksum;

private:
	FileReceiveInfo();

};

} // namespace PowerAnalyzer

#endif // PA_FILE_RECEIVE_INFO_H
