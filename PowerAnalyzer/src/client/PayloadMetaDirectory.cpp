#include "PayloadMetaDirectory.h"
#include "ClientStats.h"
#include "../../../NetAnalyzer/src/misc/MyString.h"
#include "../../../NetAnalyzer/src/Helper.h"
#include "../../../NetAnalyzer/src/Debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>

#include <fstream>
#include <sstream>

namespace {

std::vector<std::string> callbackPaths;

int ftw_callback(const char* path, const struct stat* sb, const int type) {
	(void) sb;

	if (type != FTW_F) {
		// continue, because we are only interested in regular files.
		return 0;
	}
	std::string spath(path);

	std::string sfile(spath);
	const auto itSlash = spath.find_last_of("\\/");
	if (itSlash != std::string::npos) {
		sfile = spath.substr(itSlash + 1);
	}

	const auto itDotDat = sfile.find(".dat");
	if (itDotDat == std::string::npos) {
		// wrong file type
		return 0;
	}

	sfile = sfile.substr(0, itDotDat);
	callbackPaths.push_back(sfile);
	return 0;
}

}

namespace PowerAnalyzer {

static const char* DBG_META_DIR = "META_DIR";

PayloadMetaDirectory::PayloadMetaDirectory(const std::string& dir) : metaDir(dir) {

}

PayloadMetaDirectory::~PayloadMetaDirectory() {

}

RequestMetaMap PayloadMetaDirectory::getRequestMap(ClientStats& stats) const {
	const auto contentTypes = getContentTypes();
	RequestMetaMap metaMap;

	for (const auto& ct : contentTypes) {
		collectRequestMetaInfo(ct, metaMap, stats);
	}
	return metaMap;
}

void PayloadMetaDirectory::collectRequestMetaInfo(const std::string& contentType,
												  RequestMetaMap& metaMap,
												  ClientStats& stats) const {

	debug(DBG_META_DIR, DBG_LVL_DEBUG, "adding content type: " << contentType);

	const auto datPath = metaDir + "/" + contentType + ".dat";

	std::ifstream ifs(datPath);
	if (!ifs) {
		debug(DBG_META_DIR, DBG_LVL_ERR, "failed to open .dat file: " << datPath);
		throw "Failed to open .dat file.";
	}

	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty()) {
			continue; // ignore empty lines
		}

		if (line.at(0) == '#') {
			// line starts with a # ---> it is a comment and can be ignored
			continue;
		}

		// the elements in the dat-files are seperated by tabs
		std::vector<std::string> tokens;
		const auto elemCount = strSplit(line, tokens, '\t');
		if (elemCount != 4) {
			debug(DBG_META_DIR, DBG_LVL_ERR, "line in .dat file did not have 4 elements but " << elemCount);
			throw "Invalid .dat file.";
		}

		uint64_t timestamp = 0;
		std::istringstream(tokens.at(0)) >> timestamp;
		const auto filepath = contentType + "/" + tokens.at(3);

		auto it = metaMap.find(timestamp);
		if (it == metaMap.end()) {
			// this timestamp does not exist in out map yet
			metaMap.insert({ timestamp, { filepath } });
		} else {
			it->second.push_back(filepath);
		}
		stats.filesAvailable++;
	}

	stats.contentTypes.push_back(contentType);
}

std::vector<std::string> PayloadMetaDirectory::getContentTypes() const {
	callbackPaths.clear();

	// 32 seems to be a fair figure for nopenfd.
	ftw(metaDir.c_str(), ftw_callback, 32);

	if (callbackPaths.empty()) {
		throw "the specified data directory does not contain any files.";
	}
	return callbackPaths;
}

} // namespace PowerAnalyzer
