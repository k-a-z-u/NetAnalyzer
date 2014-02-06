/*
 * PeriodicExporter.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef PERIODICEXPORTER_H_
#define PERIODICEXPORTER_H_

#include "Exporter.h"
#include "PeriodicExporterThread.h"

#include <cstdint>


class PeriodicExporter : public Exporter {

public:

	/** ctor */
	PeriodicExporter(PeriodicExporterThread* thread);

	/** dtor */
	virtual ~PeriodicExporter();

	/** run export for this exporter */
	void exportIfNeeded();


	/** set the exporting interval for this exporter */
	void setExportInterval(uint32_t seconds);

protected:

	void visit(NetSource& src) override {
		(void) src;
	}

	/** last time this exporter has been exported */
	uint64_t lastUpdateMs;

	/** the export interval in seconds */
	uint32_t exportIntervalSec;

private:

	/** returns true if it's time to run doExport() for this exporter */
	bool needsExport() const;

	/** every subclass must perform it's periodic task here */
	virtual void exportMe() = 0;


	/** the thread all PeriodicExporters are added to */
	PeriodicExporterThread* const expThread;

};

#endif /* PERIODICEXPORTER_H_ */
