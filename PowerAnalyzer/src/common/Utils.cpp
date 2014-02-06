#include "Utils.h"
#include "../common/Socket.h"

#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace PowerAnalyzer {

std::string getCompressionName(const DataCompression compr) {
	switch (compr) {
		case DataCompression::None:
			return "none";
			break;
		case DataCompression::GZip:
			return "gzip";
			break;
		case DataCompression::QuickLZ:
			return "quicklz";
			break;
		case DataCompression::LZMA_1:
			return "lzma1";
			break;
		case DataCompression::LZMA_4:
			return "lzma4";
			break;
		case DataCompression::LZO:
			return "lzo";
			break;
		case DataCompression::LZ4:
			return "lz4";
			break;
		default:
			return "invalid";
			break;
	}
}

void getCPUStat(const int cpuCore, int& user, int& nice, int& sys, int& idle, int& iowait, int& irq, int& softirq) {
	FILE* const f = fopen("/proc/stat", "r");

	if (cpuCore >= -1) {
		// skip lines from /proc/stat
		char buf[128];
		for (int i = 0; i <= cpuCore; i++) {
			fgets(buf, 128, f);
		}
	}

	fscanf(f, "%*s %d %d %d %d %d %d %d", &user, &nice, &sys, &idle, &iowait, &irq, &softirq);
	fclose(f);
}

std::string getCPUStatString(const int cpuCore) {
	int user = 0;
	int nice = 0;
	int sys = 0;
	int idle = 0;
	int iowait = 0;
	int irq = 0;
	int softirq = 0;
	getCPUStat(cpuCore, user, nice, sys, idle, iowait, irq, softirq);

	std::stringstream os;
	os << user << "\t" << nice << "\t" << sys << "\t" << idle << "\t" << iowait << "\t" << irq << "\t" << softirq;
	return os.str();
}

} // namespace PowerAnalyzer
