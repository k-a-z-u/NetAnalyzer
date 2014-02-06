#include "Options.h"
#include "../../NetAnalyzer/src/Debug.h"

#include <getopt.h>

#include <iostream>
#include <limits>
#include <cstring>

namespace PowerAnalyzer {

Options::Options() {

}

Options::~Options() {

}

void Options::printUsage(const std::string& progname) {
	std::cout << progname << " -h" << std::endl;
	std::cout << progname << " -m client"
						  << " -s SERVER"
						  << " [-p PORT]"
						  << " -d DATDIR"
						  << " [-c none|gzip|quicklz|lzma|lzo|lz4]"
						  << " [-a]"
						  << " [-l CHUNKSIZE]"
						  << " [-r REPLAYSPEED]"
						  << " [-n REPLAYCOUNT]"
						  << " [-v [debuglevel]]" << std::endl;
	std::cout << progname << " -m server"
						  << " [-p PORT]"
						  << " -d PAYLOADDIR"
						  << " [-z]"
						  << " [-v [debuglevel]]" << std::endl;
}

bool Options::parseCmdline(const int argc, char* argv[]) {
	if (argc == 1) {
		return false;
	}

	static const struct option long_options[] = {
		{ "adler",			no_argument,		nullptr, 'a' },
		{ "compression", 	required_argument, 	nullptr, 'c' },
		{ "data", 			required_argument, 	nullptr, 'd' },
		{ "help", 			no_argument, 		nullptr, 'h' },
		{ "mode", 			required_argument, 	nullptr, 'm' },
		{ "repeat", 		required_argument, 	nullptr, 'n' },
		{ "port", 			required_argument, 	nullptr, 'p' },
		{ "chunksize",		required_argument, 	nullptr, 'l' },
		{ "replay",			required_argument, 	nullptr, 'r' },
		{ "server",			required_argument,	nullptr, 's' },
		{ "verbose", 		optional_argument, 	nullptr, 'v' },
		{ "nodirectio",		no_argument,		nullptr, 'z' },
		{ nullptr, 0, nullptr, 0 }
	};

	// set default verbosity
	dbgLevel = DBG_LVL_WARN;

	while (true) {
		const auto c = getopt_long(argc, argv, "ac:d:hl:m:n:p:r:s:v::z", long_options, &optind);
		if (c == -1) {
			break;
		}

		switch (c) {
			case 'a':
				verify = true;
				break;
			case 'c':
				dataCompr = comprFromArg(optarg);
				break;
			case 'd':
				dataDir = optarg;
				break;
			case 'h':
				printUsage(argv[0]);
				return false;
			case 'l':
				chunkSize = chunkSizeFromArg(optarg);
				break;
			case 'm':
				mode = modeFromArg(optarg);
				break;
			case 'n':
				replayCount = replayCountFromArg(optarg);
				break;
			case 'p':
				port = portFromArg(optarg);
				break;
			case 'r':
				replaySpeed = replaySpeedFromArg(optarg);
				break;
			case 's':
				server = optarg;
				break;
			case 'v':
				dbgLevel = dbgLevelFromArg(optarg);
				break;
			case 'z':
				directio = false;
				break;
			case '?':
			default:
				return false;
		}
	}

	return allOptionsValid();
}

OperationMode Options::getMode() const {
	return mode;
}

std::string Options::getDataDir() const {
	return dataDir;
}

std::string Options::getServer() const {
	return server;
}

uint16_t Options::getPort() const {
	return port;
}

DataCompression Options::getCompression() const {
	return dataCompr;
}

uint32_t Options::getChunkSize() const {
	return chunkSize;
}

int Options::getReplaySpeed() const {
	return replaySpeed;
}

int Options::getReplayCount() const {
	return replayCount;
}

bool Options::verifyData() const {
	return verify;
}

bool Options::useDirectIO() const {
	return directio;
}

int Options::getCPUCore() const {
	return cpuCore;
}

bool Options::allOptionsValid() const {
	if (mode == OperationMode::Server) {
		// Check required arguments for server mode.
		if (dataDir.empty() || !port) {
			return false;
		}
	} else if (mode == OperationMode::Client) {
		// Check required arguments for client mode.
		if (server.empty() || !port) {
			return false;
		}
	} else {
		return false; // Invalid operation mode.
	}
	return true;
}

OperationMode Options::modeFromArg(const char* arg) const {
	if (strcmp(arg, "client") == 0) {
		return OperationMode::Client;
	} else if (strcmp(arg, "server") == 0) {
		return OperationMode::Server;
	} else {
		std::cout << "Invalid operation mode." << std::endl;
		return OperationMode::Invalid;
	}
}

uint16_t Options::portFromArg(const char* arg) const {
	const auto n = std::stoul(arg);
	if (n > std::numeric_limits<uint16_t>::max()) {
		std::cout << "Invalid port number." << std::endl;
	}
	return n;
}

DataCompression Options::comprFromArg(const char* arg) const {
	if (strcmp(arg, "none") == 0) {
		return DataCompression::None;
	} else if (strcmp(arg, "gzip") == 0) {
		return DataCompression::GZip;
	} else if (strcmp(arg, "quicklz") == 0) {
		return DataCompression::QuickLZ;
	} else if (strcmp(arg, "lzma1") == 0) {
		return DataCompression::LZMA_1;
	} else if (strcmp(arg, "lzma4") == 0) {
		return DataCompression::LZMA_4;
	} else if (strcmp(arg, "lzo") == 0) {
		return DataCompression::LZO;
	} else if (strcmp(arg, "lz4") == 0) {
		return DataCompression::LZ4;
	} else {
		error("PA_OPT", "Unknown compression method argument.");
		return DataCompression::None;
	}
}

uint32_t Options::chunkSizeFromArg(const char* arg) const {
	const auto n = std::stoul(arg);
	if (n > std::numeric_limits<uint32_t>::max()) {
		std::cout << "Invalid chunk size." << std::endl;
	}
	return n;
}

int Options::replaySpeedFromArg(const char* arg) const {
	const auto n = std::stoul(arg);
	if (n > std::numeric_limits<uint32_t>::max()) {
		std::cout << "Invalid replay speed." << std::endl;
	}
	return n;
}

int Options::replayCountFromArg(const char* arg) const {
	const auto n = std::stoul(arg);
	if (n > std::numeric_limits<uint32_t>::max()) {
		std::cout << "Invalid replay count." << std::endl;
	}
	return n;
}

int Options::dbgLevelFromArg(const char* arg) const {
	if (!arg) {
		return DBG_LVL_INFO; // No optional argument given. INFO is the default verbosity.
	} else {
		return std::stoul(arg);
	}
}

} // namespace PowerAnalyzer
