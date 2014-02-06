#include "ServerApp.h"
#include "ServerSocket.h"
#include "ServerStatsExporter.h"
#include "PayloadDirectory.h"
#include "../Options.h"
#include "../protocol/FileRequestInfo.h"
#include "../protocol/FileReceiveInfo.h"
#include "../common/Utils.h"
#include "../../../NetAnalyzer/src/Helper.h"
#include "../../../NetAnalyzer/src/Debug.h"

#include <arpa/inet.h>

#include <iostream>
#include <limits>
#include <cstring>

namespace PowerAnalyzer {

static const char* DBG_APP = "PWRNLYZR";

ServerApp::ServerApp(const Options& op) : options(op) {
	if (options.getCPUCore() >= -1) {
		debug(DBG_APP, DBG_LVL_DEBUG, "Binding CPU to core #" << options.getCPUCore());

		bindCurrentThreadToCore(options.getCPUCore());
		stats.cpuCore = options.getCPUCore();
	}
}

ServerApp::~ServerApp() {
	stats.dump(std::cout);

	if (!stats.outputFile.empty()) {
		stats.dump(stats.outputFile);
	}
}

void ServerApp::run() {
	debug(DBG_APP, DBG_LVL_DEBUG, "server starting to listen on port " << options.getPort());

	PayloadDirectory dir(options.getDataDir());
	stats.dataDir = options.getDataDir();

	ServerStatsExporter exporter(options, stats);
	exporter.start();

	stats.outputFile = exporter.getExportFilename() + ".log";

	ServerSocket server;
	start(server);

	while (true) {
		auto client = server.accept();
		debug(DBG_APP, DBG_LVL_DEBUG, "client connected to server");

		serveClient(client, dir);
		debug(DBG_APP, DBG_LVL_INFO, "the client got served and is about to be disconnected");
	}
}

void ServerApp::stop() {
	// TODO: stop running server. use some bool or event
}

void ServerApp::start(ServerSocket& server) {
	server.bind(options.getPort());
	server.listen();
	stats.startTime = getTimeStampMS();
}

void ServerApp::serveClient(Socket& client, PayloadDirectory& dir) {
	const auto info = recvRequestInfo(client);
	debug(DBG_APP, DBG_LVL_DEBUG, "client requests " << info.filename << " (chunksize=" << info.chunkSize << ")");

	// log the timestamp of the current file
	stats.lastFileTimestamp = timestampFromPath(info.filename);

	// read the requested file chunk by chunk from the payload dir,
	// compress it and send the compressed data to the client
	sendFile(info, dir, client);

	// clients might request multiple files in the future.
	stats.clientsServed++;
}

FileRequestInfo ServerApp::recvRequestInfo(Socket& client) {
	const auto infoBuf = client.recv(FileRequestInfo::PROTOCOL_SIZE);
	stats.bytesReceived += infoBuf.size();

	return FileRequestInfo::fromProtocolBuffer(infoBuf);
}

void ServerApp::sendFile(const FileRequestInfo& info, PayloadDirectory& dir, Socket& client) {
	const auto calcChecksum = (info.flags & FILE_REQUEST_CHECKSUM);
	dir.openFile(info.filename, info.compression, options.useDirectIO());

	while (!dir.fileDone()) {
		uint32_t checksum = 0;
		const auto buf = dir.readFile(info.chunkSize, calcChecksum, checksum, stats);

		if (buf.empty()) {
			// it is possible that the compressor did not yet return compressed data.
			// this is true especially for streaming compressors like gzip.
			continue;
		}
		debug(DBG_APP, DBG_LVL_INFO, "about to send " << buf.size() << " bytes of payload to the client");

		// send the compressed data to the connected client
		const auto bytesSent = sendFileChunk(buf, checksum, client);

		debug(DBG_APP, DBG_LVL_INFO, bytesSent << " bytes were sent to the client in total");
		stats.payloadSent += buf.size();
		stats.bytesSent += bytesSent;
	}

	// handle last chunk of data that might still be in the compression stream
	uint32_t checksum = 0;
	const auto buf = dir.getRemainingData(calcChecksum, checksum, stats);
	if (!buf.empty()) {
		debug(DBG_APP, DBG_LVL_INFO, "about to send the last " << buf.size() << " bytes of payload to the client");

		// send the compressed data to the connected client
		const auto bytesSent = sendFileChunk(buf, checksum, client);
		debug(DBG_APP, DBG_LVL_INFO, bytesSent << " bytes were sent to the client in total");

		stats.payloadSent += buf.size();
		stats.bytesSent += bytesSent;
	}

	// send end-of-file indicator
	const auto bytesSent = sendFileChunk({ }, 0, client);
	debug(DBG_APP, DBG_LVL_INFO, "another " << bytesSent << " bytes were sent to indicate the end of file");

	stats.bytesSent += bytesSent;
	stats.filesSent++;
}

size_t ServerApp::sendFileChunk(const std::vector<char>& chunk, const uint32_t checksum, Socket& client) {
	if (chunk.size() > std::numeric_limits<uint32_t>::max()) {
		throw "chunk size too large, it does not fit into an uint32_t";
	}

	const auto info = FileReceiveInfo::toProtocolBuffer(chunk.size(), checksum);

	client.send(info);
	client.send(chunk);

	return (FileReceiveInfo::PROTOCOL_SIZE + chunk.size());
}

std::string ServerApp::timestampFromPath(const std::string& path) const {
	std::string timestamp(path);

	const auto itSlash = path.find_last_of("\\/");
	if (itSlash != std::string::npos) {
		timestamp = path.substr(itSlash + 1);
	}

	const auto itUnderscore = timestamp.find("_");
	if (itUnderscore == std::string::npos) {
		return "0";
	}

	timestamp = timestamp.substr(0, itUnderscore);
	return timestamp;
}

} // namespace PowerAnalyzer
