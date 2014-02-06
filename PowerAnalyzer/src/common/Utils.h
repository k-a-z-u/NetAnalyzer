#ifndef PA_UTILS_H
#define PA_UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace PowerAnalyzer {

class Socket;

/** Specifiy enum type because it will be send over the network. */
enum class DataCompression : uint16_t {
	None,
	GZip,
	QuickLZ,
	LZMA_1,
	LZMA_4,
	LZO,
	LZ4
};

std::string getCompressionName(DataCompression compr);

void getCPUStat(int cpuCore, int& user, int& nice, int& sys, int& idle, int& iowait, int& irq, int& softirq);
std::string getCPUStatString(int cpuCore);

} // namespace PowerAnalyzer

#endif // PA_UTILS_H
