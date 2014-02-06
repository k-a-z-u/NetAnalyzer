/*
 * Thread.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "../Debug.h"

#include <pthread.h>

#define DBG_THREAD			"THREAD"

class Thread {

public:

	/** ctor */
	Thread() : enabled(true), handle(0) {

	}

	/** dtor */
	virtual ~Thread() {
		stop();
	}

	/** start the thread */
	void start() {
		if (handle != 0) {return;}
		debug(DBG_THREAD, DBG_LVL_INFO, "starting");
		enabled = true;
		pthread_create(&handle, 0, threadCallback, this);
	}

	/** kill this thread */
	void stop() {
		if (handle == 0) {return;}
		debug(DBG_THREAD, DBG_LVL_INFO, "stopping");
		enabled = false;
		//pthread_cancel(handle);					// seems like this will lead to pure-virtual-method calls...
		pthread_join(handle, 0);				// needed for proper cleanup (http://stackoverflow.com/questions/5610677/valgrind-memory-leak-errors-when-using-pthread-create)
		handle = 0;
	}

	/** wait for this thread to join */
	void join() {
		if (handle == 0) {return;}
		debug(DBG_THREAD, DBG_LVL_INFO, "joining");
		pthread_join(handle, 0);
	}

	/** the runnable code */
	virtual void run() = 0;

protected:

	/** child MUST ONLY run whil this variable is true */
	bool enabled;

private:

	/** callback to run the thread's code */
	static void* threadCallback(void* clazz) {
		Thread* thread = (Thread*) clazz;
		thread->run();
		pthread_exit(0);
	}

	/** the thread handle */
	pthread_t handle;

};


#endif /* THREAD_H_ */
