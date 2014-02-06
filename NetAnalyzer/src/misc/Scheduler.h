/*
 * Scheduler.h
 *
 *  Created on: May 2, 2013
 *      Author: kazu
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Thread.h"
#include "../Helper.h"
#include <cstdint>
#include <unistd.h>

/**
 * special form of Thread that executes at given intervals
 */
class Scheduler : public Thread {

public:

	/** ctor */
	Scheduler() : intervalMs(1000) {;}

	/** set the execution interval */
	void setIntervalMs(uint32_t intervalMs) {
		this->intervalMs = intervalMs;
	}

	void run() override {

		while(enabled) {

			// execute tasks and meassure the needed time
			uint64_t start = getTimeStampMS();
			execTasks();
			uint64_t end = getTimeStampMS();

			// calculate the time needed for execution
			// and then calculate the sleep-time before next run
			int64_t execTimeMs = end-start;
			int64_t sleepMs = intervalMs - execTimeMs;

			// sleep until next execution
			if (sleepMs > 0) {
				usleep(sleepMs * 1000);
			}

		}

	}

	/** subclasses must execute their tasks here */
	virtual void execTasks() = 0;

private:

	/** the interval for the scheduler */
	int64_t intervalMs;

};



#endif /* SCHEDULER_H_ */
