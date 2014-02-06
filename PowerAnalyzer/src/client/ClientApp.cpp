#include "ClientApp.h"
#include "ClientSocket.h"
#include "ClientStatsExporter.h"
#include "PayloadMetaDirectory.h"
#include "../protocol/FileRequestInfo.h"
#include "../protocol/FileReceiveInfo.h"
#include "../common/Utils.h"
#include "../../../NetAnalyzer/src/compression/Data.h"
#include "../../../NetAnalyzer/src/compression/GZipDecompress.h"
#include "../../../NetAnalyzer/src/compression/LZMADecompress.h"
#include "../../../NetAnalyzer/src/compression/QuickLZDecompress.h"
#include "../../../NetAnalyzer/src/compression/LZ4Decompress.h"
#include "../../../NetAnalyzer/src/compression/LZODecompress.h"
#include "../../../NetAnalyzer/src/Helper.h"
#include "../../../NetAnalyzer/src/Debug.h"

#include <zlib.h>

#include <chrono>
#include <iostream>
#include <thread>

namespace PowerAnalyzer {

static const char* DBG_APP = "PWRNLYZR";

ClientApp::ClientApp(const Options& op) : options(op) {
	if (options.getCPUCore() >= -1) {
		debug(DBG_APP, DBG_LVL_DEBUG, "Binding CPU to core #" << options.getCPUCore());

		bindCurrentThreadToCore(options.getCPUCore());
		stats.cpuCore = options.getCPUCore();
	}

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: None");
	decompressors.insert(std::make_pair(DataCompression::None, nullptr));

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: GZipDecompress");
	decompressors.insert(std::make_pair(DataCompression::GZip, make_unique<GZipDecompress>()));

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: QuickLZDecompress");
	decompressors.insert(std::make_pair(DataCompression::QuickLZ, make_unique<QuickLZDecompress>()));

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: LZMADecompress");
	decompressors.insert(std::make_pair(DataCompression::LZMA_1, make_unique<LZMADecompress>()));
	decompressors.insert(std::make_pair(DataCompression::LZMA_4, make_unique<LZMADecompress>()));

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: LZODecompress");
	decompressors.insert(std::make_pair(DataCompression::LZO, make_unique<LZODecompress>()));

	debug(DBG_APP, DBG_LVL_INFO, "Loading decompressor: LZ4Decompress");
	decompressors.insert(std::make_pair(DataCompression::LZ4, make_unique<LZ4Decompress>()));
}

ClientApp::~ClientApp() {
	stats.dump(std::cout);

	if (!stats.outputFile.empty()) {
		stats.dump(stats.outputFile);
	}
}

void ClientApp::run() {
	// get all the client requests that will be made by this program
	const auto requests = readClientRequests();

	waitForStart();

	ClientStatsExporter exporter(options, stats);
	exporter.start();

	stats.outputFile = exporter.getExportFilename() + ".log";

	// the timestamp that the client was started
	const auto startTimestamp = getTimeStampMS();

	// the first item will have the lowest timestamp of all requests
	const auto firstTimestamp = requests.cbegin()->first;
	stats.potentialEndTime = startTimestamp + (requests.crbegin()->first - firstTimestamp);

	runRequestLoop(startTimestamp, firstTimestamp, requests);
}

void ClientApp::stop() {
	// TODO: stop running client: gracefully stop the run() method and
	// clean up its stack variables. the object is probably about to be destroyed.
}

RequestMetaMap ClientApp::readClientRequests() const {
	PayloadMetaDirectory dir(options.getDataDir());
	stats.metaDir = options.getDataDir();
	stats.replaySpeed = options.getReplaySpeed();

	const auto requestMap = dir.getRequestMap(stats);
	if (requestMap.empty()) {
		throw "no files to request";
	}

	stats.uniqueTimestamps = requestMap.size();
	return requestMap;
}

void ClientApp::waitForStart() const {
	std::cout << "All .dat files read. Press ENTER to start requests.";
	std::cin.get();
	std::cout << "Fire in the hole!" << std::endl;

	stats.startTime = getTimeStampMS();
}

void ClientApp::runRequestLoop(const uint64_t clientStartTime,
							   const uint64_t firstRequestTime,
							   const RequestMetaMap& requests) {
	// msTooLate contains the amount of milliseconds that the current request
	// is too late compared to the original timestamp. this can happen if
	// the 'repeat' parameter is too high, the replay speed is too fast,
	// or the compression takes too long. the application will try to keep up
	// by reducing the wait-time between repeated requests.
	int msTooLate = 0;

	// used to determine 'percentage-done'-debug info
	int timestampsDone = 0;

	for (RequestMetaMap::const_iterator requestIter = std::begin(requests);
		 requestIter != std::end(requests);
		 requestIter++) {

		// this makes sure that we do not send the current request too early.
		// it is possible, however, that we are too late.
		msTooLate = waitTillRequest(clientStartTime, firstRequestTime, requestIter->first);
		stats.tooLateTime = msTooLate;

		auto waitTimeRepeat = calcRepeatWaitTime(requestIter, std::end(requests), msTooLate);

		// send the request as ofter as specified and wait the appropriate
		// amount of time between the requests.
		handleRepeatedRequest(requestIter, waitTimeRepeat);

		// update this only after the first timestamp was served so that
		// there is already stats data available
		stats.lastFileTimestamp = requestIter->first;

		const float percentDone = (100.f / requests.size() * ++timestampsDone);
		debug(DBG_APP, DBG_LVL_DEBUG, percentDone << "% done (" << timestampsDone << " / " << requests.size() << " requests)");
	}

	debug(DBG_APP, DBG_LVL_DEBUG, "the whole request loop finished " << msTooLate << "ms too late.");
}

int ClientApp::waitTillRequest(const uint64_t clientStartTime,
							   const uint64_t firstRequestTime,
							   const uint64_t requestTime) const {
	// the number of milliseconds passed since the client started
	const auto clientUptime = getTimeStampMS() - clientStartTime;

	// the number of milliseconds passed relative to the first request
	const auto nextRequestTime = (requestTime - firstRequestTime) / options.getReplaySpeed();

	if (clientUptime < nextRequestTime) {
		// wait until the next file request is due
		const auto diffMs = nextRequestTime - clientUptime;
		debug(DBG_APP, DBG_LVL_DEBUG, "next request is due in " << diffMs << "ms. waiting...");

		// wait till it is time to send the current request
		std::this_thread::sleep_for(std::chrono::milliseconds(diffMs));
		stats.sleepTime += diffMs;

		// the current request is in time.
		return 0;
	} else {
		// tell the caller how many milliseconds we are behind schedule
		// for the current request.
		return (clientUptime - nextRequestTime);
	}
}

int ClientApp::calcRepeatWaitTime(RequestMetaMap::const_iterator& requestIter,
								  const RequestMetaMap::const_iterator& requestsEnd,
								  const int msTooLate) {
	if (options.getReplayCount() > 1) {
		// we want to send each file a certain amount of times.
		// calculate how long we should wait between each time we send it.
		// distribute the wait time for each request-repeatition over the
		// time the program would have to wait for the next request.

		// but also take into account that we might be too late. in that case
		// reduce the wait-time between repeated requests.

		const auto nextRequestIter = ++requestIter;
		--requestIter; // reset iterator state

		if (nextRequestIter != requestsEnd) {
			// original time between this and the next request
			int msTillNextRequest = (nextRequestIter->first - requestIter->first) / options.getReplaySpeed();

			// reduce the time we have till the next request by the time
			// we are already too late. do not let it become negative.
			if (msTooLate > 0) {
				msTillNextRequest = ((msTillNextRequest - msTooLate) > 0) ? (msTillNextRequest - msTooLate) : 0;
				debug(DBG_APP, DBG_LVL_INFO, "the current request is " << msTooLate << "ms too late. repetition wait time will be adjusted");
			}

			auto repeatWaitTime = msTillNextRequest / options.getReplayCount();
			debug(DBG_APP, DBG_LVL_INFO, "repetition wait time is (" << msTillNextRequest << " / " << options.getReplayCount() << ") = " << repeatWaitTime << "ms");
			return repeatWaitTime;
		}
	}
	return 0;
}

void ClientApp::handleRepeatedRequest(const RequestMetaMap::const_iterator& requestIter, const int waitTimeRepeat) {
	for (int repeatCnt = 0; repeatCnt < options.getReplayCount(); repeatCnt++) {

		// consider the time it took to handle the last request as wait time
		const auto requestTime = handleRequest(requestIter->second);
		debug(DBG_APP, DBG_LVL_INFO, "last request took " << requestTime << "ms");

		auto realWaitTimeRepeat = 0;
		if (requestTime < waitTimeRepeat) {
			realWaitTimeRepeat = waitTimeRepeat - requestTime;
			debug(DBG_APP, DBG_LVL_INFO, "next file request is due in " << realWaitTimeRepeat << "ms");
		}

		// wait till it is time to send the current request again
		std::this_thread::sleep_for(std::chrono::milliseconds(realWaitTimeRepeat));
		stats.sleepTime += realWaitTimeRepeat;
	}
}

int ClientApp::handleRequest(const std::vector<std::string>& filenames) {
	const auto requestStartTime = getTimeStampMS();

	// request each file of the current timestamp individually
	for (const auto& filename : filenames) {

		ClientSocket client;
		connect(client);

		// request the current file from the server
		requestFile(client, filename);

		// receive the requested file data from the server
		receiveFile(client);
	}

	// return the time it took to handle the request
	return (getTimeStampMS() - requestStartTime);
}

void ClientApp::connect(ClientSocket& client) {
	debug(DBG_APP, DBG_LVL_INFO, "trying to connect to server: " << options.getServer() << ":" << options.getPort());
	client.connect(options.getServer(), options.getPort());
	stats.connections++;
}

void ClientApp::requestFile(ClientSocket& client, const std::string& filename) {
	debug(DBG_APP, DBG_LVL_INFO, "sending request for " << filename << " (chunksize=" << options.getChunkSize() << ", checksum=" << options.verifyData() << ")");
	const auto request = FileRequestInfo::toProtocolBuffer(filename,
														   options.getCompression(),
														   options.getChunkSize(),
														   options.verifyData());
	client.send(request);
	stats.bytesSent += request.size();
	stats.filesRequested++;

	// compression is set on file level. select the decompression accordingly
	activeDecompressor = decompressors[options.getCompression()].get();
}

void ClientApp::receiveFile(ClientSocket& client) {
	// receive the file chunk after chunk till it is finished
	while (true) {
		// the size of the following chunk is sent first
		const auto infoBuf = client.recv(FileReceiveInfo::PROTOCOL_SIZE);
		stats.bytesReceived += infoBuf.size();

		const auto info = FileReceiveInfo::fromProtocolBuffer(infoBuf);
		debug(DBG_APP, DBG_LVL_INFO, "received: next chunk will be " << info.chunkSize << " bytes large");

		if (info.chunkSize == 0) {
			// no more data for the current file. move on to the next file
			debug(DBG_APP, DBG_LVL_INFO, "no more data for the current file. moving on to the next file");
			break;
		}
		const auto chunk = client.recv(info.chunkSize);

		stats.bytesReceived += chunk.size();
		stats.payloadReceived += chunk.size();
		debug(DBG_APP, DBG_LVL_INFO, "received: chunk of " << chunk.size() << " bytes");

		// verify the compressed data
		verifyChunk(chunk, info.checksum);

		decompressChunk(chunk);
	}

	// the decompressor should be reset after each file
	if (activeDecompressor) {
		activeDecompressor->reset();
	}
}

void ClientApp::verifyChunk(const std::vector<char>& chunk, const uint32_t checksum) const {
	if (!options.verifyData()) {
		return;
	}

	const auto start = getTimeStampUS();
	const uint32_t initAdler = adler32(0L, Z_NULL, 0);
	const uint32_t chunkChecksum = adler32(initAdler, (unsigned char*)chunk.data(), chunk.size());
	const auto stop = getTimeStampUS();
	stats.checksumTimeUs += (stop - start);

	if (chunkChecksum == checksum) {
		debug(DBG_APP, DBG_LVL_INFO, "uncompressed data successfully verified (checksum=" << chunkChecksum << ")");
	} else {
		error(DBG_APP, "unable to verify uncompressed data (client=" << chunkChecksum << " <-> server=" << checksum << ")");
	}
}

void ClientApp::decompressChunk(const std::vector<char>& chunk) const {
	if (!activeDecompressor) {
		if (options.getCompression() != DataCompression::None) {
			// the decompressor for the selected compression method is not (yet?)
			// available. do not shutdown at this point but print a warning.
			debug(DBG_APP, DBG_LVL_ERR, "decompressor not available for the selected compression method!");
		}
		return;
	}
	const auto start = getTimeStampUS();

	Data compressed((const uint8_t*)chunk.data(), chunk.size());
	std::vector<uint8_t> uncompressed;
	activeDecompressor->append(compressed, uncompressed);

	const auto stop = getTimeStampUS();
	stats.decompressionTimeUs += (stop - start);

	stats.bytesDecompressedPayload += uncompressed.size();
}

} // namespace PowerAnalyzer
