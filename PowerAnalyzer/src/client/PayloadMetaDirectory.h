#ifndef PA_PAYLOAD_META_DIR_H
#define PA_PAYLOAD_META_DIR_H

#include <map>
#include <string>
#include <vector>
#include <cstdint>

namespace PowerAnalyzer {

class ClientStats;

typedef std::map<uint64_t, std::vector<std::string>> RequestMetaMap;

class PayloadMetaDirectory {
public:
	PayloadMetaDirectory(const std::string& dir);
	~PayloadMetaDirectory();

	RequestMetaMap getRequestMap(ClientStats& stats) const;

private:
	void collectRequestMetaInfo(const std::string& contentType, RequestMetaMap& metaMap, ClientStats& stats) const;
	std::vector<std::string> getContentTypes() const;

	const std::string metaDir;
};

} // namespace PowerAnalyzer

#endif // PA_PAYLOAD_META_DIR_H
