/*
 * PeriodicExporter.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#include "PeriodicExporter.h"
#include "../Helper.h"

#include <cstdint>

PeriodicExporter::PeriodicExporter(PeriodicExporterThread* thread) : exportIntervalSec(60), expThread(thread) {
	lastUpdateMs = getTimeStampMS();
	expThread->add(this);
}

PeriodicExporter::~PeriodicExporter() {
	expThread->remove(this);
}

void PeriodicExporter::exportIfNeeded() {
	if (!needsExport()) {return;}
	lastUpdateMs = getTimeStampMS();
	exportMe();
}

void PeriodicExporter::setExportInterval(uint32_t seconds) {
	exportIntervalSec = seconds;
}

bool PeriodicExporter::needsExport() const {
	return (getTimeStampMS() - lastUpdateMs) > (1000 * exportIntervalSec);
}
