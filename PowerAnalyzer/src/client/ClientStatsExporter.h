#ifndef PA_CLIENT_STATS_EXPORTER_H
#define PA_CLIENT_STATS_EXPORTER_H

#include "../../../NetAnalyzer/src/misc/Scheduler.h"

#include "../Options.h"

#include <string>
#include <fstream>

namespace PowerAnalyzer {

class ClientStats;

class ClientStatsExporter : public Scheduler {
public:
	ClientStatsExporter(const Options& op, const ClientStats& stats);
	virtual ~ClientStatsExporter();

	void execTasks() override;

	std::string getExportFilename() const;

private:
	void printOptions(std::ostream& os);
	void printCurrentStats(std::ostream& os);

	const Options& options;
	const ClientStats& stats;
	std::ofstream os;
};

} // namespace PowerAnalyzer

#endif // PA_CLIENT_STATS_EXPORTER_H
