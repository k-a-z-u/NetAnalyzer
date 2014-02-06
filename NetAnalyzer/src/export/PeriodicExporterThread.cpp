/*
 * PeriodicExporterThread.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#include "PeriodicExporterThread.h"

#include "../Debug.h"
#include "PeriodicExporter.h"

#define DBG_EXP_THREAD			"ExThread"

PeriodicExporterThread::PeriodicExporterThread() {

	// configure the export interval
	setIntervalMs(1000);

}

PeriodicExporterThread::~PeriodicExporterThread() {

}

void PeriodicExporterThread::add(PeriodicExporter* exp) {
	debug(DBG_EXP_THREAD, DBG_LVL_INFO, "added one new PeriodicExporter to thread-pool");
	exporters.push_back(exp);
}

void PeriodicExporterThread::remove(PeriodicExporter* exp) {
	debug(DBG_EXP_THREAD, DBG_LVL_INFO, "removed one PeriodicExporter");
	for (auto it = exporters.begin(); it != exporters.end(); ++it) {
		if(*it == exp) {exporters.erase(it); break;}
	}
//	if (exporters.empty()) {
//		debug(DBG_EXP_THREAD, DBG_LVL_INFO, "all PeriodicExporters removed. shutting down!");
//		delete this;
//	}
}

void PeriodicExporterThread::execTasks() {
	for (auto exp : exporters) {exp->exportIfNeeded();}
}
