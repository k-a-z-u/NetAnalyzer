#include "FileRequestInfo.h"

#include <arpa/inet.h>

#include <cstring>

namespace PowerAnalyzer {

FileRequestInfo FileRequestInfo::fromProtocolBuffer(const std::vector<char>& buf) {
	if (buf.size() != PROTOCOL_SIZE) {
		throw "Invalid buffer size. This is no valid FileRequestInfo.";
	}
	size_t offset = 0;

	std::vector<char> filenameBuf(FILENAME_LEN, 0);
	std::memcpy(filenameBuf.data(), buf.data() + offset, FILENAME_LEN);
	filenameBuf.push_back('\0'); // the 0 terminator is not part of the protocol
	offset += FILENAME_LEN;

	uint16_t compressionBuf = 0;
	std::memcpy(&compressionBuf, buf.data() + offset, sizeof(uint16_t));
	offset += sizeof(uint16_t);

	uint32_t chunkSizeBuf = 0;
	std::memcpy(&chunkSizeBuf, buf.data() + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	uint8_t flags = 0;
	std::memcpy(&flags, buf.data() + offset, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	if (offset != PROTOCOL_SIZE) {
		throw "Failed to convert protocol buffer to FileRequestInfo.";
	}

	const std::string filename(filenameBuf.data());
	const DataCompression compression = static_cast<DataCompression>(ntohs(compressionBuf));
	const uint32_t chunkSize = ntohl(chunkSizeBuf);

	FileRequestInfo info;
	info.filename = filename;
	info.compression = compression;
	info.chunkSize = chunkSize;
	info.flags = flags;
	return info;
}

std::vector<char> FileRequestInfo::toProtocolBuffer(const std::string& filename,
													const DataCompression compression,
													const uint32_t chunkSize,
													const bool checksum) {

	if (filename.size() > FILENAME_LEN) {
		throw "Filename to large!";
	}

	std::vector<char> protocol(PROTOCOL_SIZE, 0);
	size_t offset = 0;

	std::memcpy(protocol.data() + offset, filename.data(), filename.size());
	offset += FILENAME_LEN;

	const uint16_t compressionBuf = htons(static_cast<uint16_t>(compression));
	std::memcpy(protocol.data() + offset, &compressionBuf, sizeof(uint16_t));
	offset += sizeof(uint16_t);

	const uint32_t chunkSizeBuf = htonl(chunkSize);
	std::memcpy(protocol.data() + offset, &chunkSizeBuf, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	uint8_t flagsBuf = 0;
	if (checksum) {
		flagsBuf |= FILE_REQUEST_CHECKSUM;
	}
	std::memcpy(protocol.data() + offset, &flagsBuf, sizeof(uint8_t));
	offset += sizeof(uint8_t);

	if (offset != PROTOCOL_SIZE) {
		throw "Failed to create FileRequestInfo protocol buffer.";
	}
	return protocol;
}

FileRequestInfo::FileRequestInfo() {

}

} // namespace PowerAnalyzer
