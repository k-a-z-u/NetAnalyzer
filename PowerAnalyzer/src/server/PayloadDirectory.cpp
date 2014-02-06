#include "PayloadDirectory.h"
#include "ServerStats.h"
#include "../../../NetAnalyzer/src/compression/GZipCompress.h"
#include "../../../NetAnalyzer/src/compression/QuickLZCompress.h"
#include "../../../NetAnalyzer/src/compression/LZMACompress.h"
#include "../../../NetAnalyzer/src/compression/LZOCompress.h"
#include "../../../NetAnalyzer/src/compression/LZ4Compress.h"
#include "../../../NetAnalyzer/src/compression/Data.h"
#include "../../../NetAnalyzer/src/Helper.h"
#include "../../../NetAnalyzer/src/Debug.h"

// used for adler32 checksum
#include <zlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace PowerAnalyzer {

static const char* DBG_DATA_DIR = "DATA_DIR";

PayloadDirectory::PayloadDirectory(const std::string& dir) : dataDir(dir) {
	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: None");
	compressors.insert(std::make_pair(DataCompression::None, nullptr));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: GZipCompress");
	compressors.insert(std::make_pair(DataCompression::GZip, make_unique<GZipCompress>()));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: QuickLZCompress");
	compressors.insert(std::make_pair(DataCompression::QuickLZ, make_unique<QuickLZCompress>()));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: LZMACompress(lvl=1)");
	compressors.insert(std::make_pair(DataCompression::LZMA_1, make_unique<LZMACompress>(1)));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: LZMACompress(lvl=4)");
	compressors.insert(std::make_pair(DataCompression::LZMA_4, make_unique<LZMACompress>(4)));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: LZOCompress");
	compressors.insert(std::make_pair(DataCompression::LZO, make_unique<LZOCompress>()));

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Loading compressor: LZ4Compress");
	compressors.insert(std::make_pair(DataCompression::LZ4, make_unique<LZ4Compress>()));
}

PayloadDirectory::~PayloadDirectory() {

}

void PayloadDirectory::openFile(const std::string& filepath, const DataCompression compression, const bool directio) {
	// close the currently open file if there is one
	if (fileDescriptor != -1) {
		close(fileDescriptor);

		fileDescriptor = -1;
		fileDescriptorEOF = false;
		alignmentNeeded = 0;
	}

	const auto fullPath = dataDir + "/" + filepath;
	debug(DBG_DATA_DIR, DBG_LVL_INFO, "Opening file " << fullPath);

	if (directio) {
		fileDescriptor = open(fullPath.c_str(), O_RDONLY | O_DIRECT);
	} else {
		fileDescriptor = open(fullPath.c_str(), O_RDONLY);
	}
	if (fileDescriptor == -1) {
		debug(DBG_DATA_DIR, DBG_LVL_ERR, "failed to open .dat file: " << fullPath);
		throw "Failed to open .dat file.";
	}
	alignmentNeeded = fpathconf(fileDescriptor, _PC_REC_XFER_ALIGN);

	activeCompressor = compressors[compression].get();
	if (activeCompressor) {
		debug(DBG_DATA_DIR, DBG_LVL_INFO, "Active compressor is: " << activeCompressor->getName());
		activeCompressor->open();
	} else {
		debug(DBG_DATA_DIR, DBG_LVL_INFO, "Active compressor is: None");
	}
}

std::vector<char> PayloadDirectory::readFile(const uint32_t chunkSize,
											 const bool needChecksum,
											 uint32_t& checksum,
											 ServerStats& stats) {
	if (fileDescriptor == -1) {
		debug(DBG_DATA_DIR, DBG_LVL_ERR, "The file not ready to be read.");
		throw "File not ready to be read.";
	}

	void* chunkBuf = nullptr;
	const auto rcAlign = posix_memalign(&chunkBuf, alignmentNeeded, chunkSize);
	if (rcAlign != 0) {
		throw "posix_memalign() failed!";
	}

	const auto count = read(fileDescriptor, chunkBuf, chunkSize);
	if (count < 0) {
		throw "read() failed!";
	}
	stats.bytesRead += count;

	if (count != chunkSize) {
		// ignore situations like 'interrupted by signal' for now
		fileDescriptorEOF = true;
	}

	std::vector<char> chunk(count);
	std::memcpy(chunk.data(), chunkBuf, chunk.size());

	free(chunkBuf);
	chunkBuf = nullptr;

	if (activeCompressor) {
		const auto start = getTimeStampUS();
		chunk = compressData(chunk);
		const auto stop = getTimeStampUS();
		stats.compressionTimeUs += (stop - start);
	}

	if (needChecksum && !chunk.empty()) {
		checksum = calcChecksum(chunk, stats);
	}
	return chunk;
}

bool PayloadDirectory::fileDone() const {
	return fileDescriptorEOF;
}

std::vector<char> PayloadDirectory::getRemainingData(const bool needChecksum,
													 uint32_t& checksum,
													 ServerStats& stats) {
	std::vector<char> compressed;
	if (activeCompressor) {
		const auto start = getTimeStampUS();
		activeCompressor->close(compressed);
		const auto stop = getTimeStampUS();
		stats.compressionTimeUs += (stop - start);
	}

	if (needChecksum && !compressed.empty()) {
		checksum = calcChecksum(compressed, stats);
		debug(DBG_DATA_DIR, DBG_LVL_INFO, "checksum of last file chunk: " << checksum);
	}
	return compressed;
}

std::vector<char> PayloadDirectory::compressData(const std::vector<char>& data) const {
	Data uncompressed((uint8_t*)data.data(), data.size());
	std::vector<char> compressed;

	activeCompressor->append(uncompressed, compressed);
	return compressed;
}

uint32_t PayloadDirectory::calcChecksum(const std::vector<char>& data, ServerStats& stats) {
	const auto start = getTimeStampUS();

	// calculate the adler32 checksum if the client requested it
	const uint32_t initAdler = adler32(0L, Z_NULL, 0);
	const uint32_t checksum = adler32(initAdler, (unsigned char*)data.data(), data.size());

	const auto stop = getTimeStampUS();
	stats.checksumTimeUs += (stop - start);

	debug(DBG_DATA_DIR, DBG_LVL_INFO, "checksum of file chunk: " << checksum);
	return checksum;
}

} // namespace PowerAnalyzer
