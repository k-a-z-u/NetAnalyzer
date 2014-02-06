/*
 * PeriodicExporterThread.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef PERIODICEXPORTERTHREAD_H_
#define PERIODICEXPORTERTHREAD_H_

#include "../misc/Scheduler.h"

#include <vector>

// forward declarations
class PeriodicExporter;

/**
 * the thread to use for all periodic exporters
 */
class PeriodicExporterThread : public Scheduler {

public:

	/** ctor */
	PeriodicExporterThread();

	/** dtor */
	virtual ~PeriodicExporterThread();

	/** add a new exporter. the object will not take ownership of the pointer! */
	void add(PeriodicExporter* exp);

	/**
	 * remove one exporter from this thread
	 * the object behind the pointer will not be deleted.
	 */
	void remove(PeriodicExporter* exp);

	void execTasks() override;

private:
	/** store all exporters to export */
	std::vector<PeriodicExporter*> exporters;

	PeriodicExporterThread(const PeriodicExporterThread&) = delete;
	PeriodicExporterThread& operator=(const PeriodicExporterThread&) = delete;

};

#endif /* PERIODICEXPORTERTHREAD_H_ */
