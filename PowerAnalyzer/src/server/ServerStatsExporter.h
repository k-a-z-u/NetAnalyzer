#ifndef PA_SERVER_STATS_EXPORTER_H
#define PA_SERVER_STATS_EXPORTER_H

#include "../../../NetAnalyzer/src/misc/Scheduler.h"

#include "../Options.h"

#include <string>
#include <fstream>

namespace PowerAnalyzer {

class ServerStats;

class ServerStatsExporter : public Scheduler {
public:
	ServerStatsExporter(const Options& op, const ServerStats& stats);
	virtual ~ServerStatsExporter();

	void execTasks() override;

	std::string getExportFilename() const;

private:
	void printOptions(std::ostream& os);
	void printCurrentStats(std::ostream& os);

	const Options& options;
	const ServerStats& stats;
	std::ofstream os;
};

} // namespace PowerAnalyzer

#endif // PA_SERVER_STATS_EXPORTER_H
