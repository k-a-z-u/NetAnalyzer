#include "FileReceiveInfo.h"

#include <arpa/inet.h>

#include <cstring>

namespace PowerAnalyzer {

FileReceiveInfo FileReceiveInfo::fromProtocolBuffer(const std::vector<char>& buf) {
	if (buf.size() != PROTOCOL_SIZE) {
		throw "Invalid buffer size. This is no valid FileRequestInfo.";
	}

	uint32_t chunkSizeBuf = 0;
	std::memcpy(&chunkSizeBuf, buf.data(), sizeof(uint32_t));
	const uint32_t chunkSize = ntohl(chunkSizeBuf);

	uint32_t checksumBuf = 0;
	std::memcpy(&checksumBuf, buf.data() + sizeof(uint32_t), sizeof(uint32_t));
	const uint32_t checksum = ntohl(checksumBuf);

	FileReceiveInfo info;
	info.chunkSize = chunkSize;
	info.checksum = checksum;
	return info;
}

std::vector<char> FileReceiveInfo::toProtocolBuffer(const uint32_t chunkSize) {
	return toProtocolBuffer(chunkSize, 0);
}

std::vector<char> FileReceiveInfo::toProtocolBuffer(const uint32_t chunkSize, const uint32_t checksum) {

	std::vector<char> protocol(PROTOCOL_SIZE, 0);

	const uint32_t chunkSizeBuf = htonl(chunkSize);
	std::memcpy(protocol.data(), &chunkSizeBuf, sizeof(uint32_t));

	const uint32_t checksumBuf = htonl(checksum);
	std::memcpy(protocol.data() + sizeof(uint32_t), &checksumBuf, sizeof(uint32_t));

	return protocol;
}

FileReceiveInfo::FileReceiveInfo() {

}

} // namespace PowerAnalyzer
