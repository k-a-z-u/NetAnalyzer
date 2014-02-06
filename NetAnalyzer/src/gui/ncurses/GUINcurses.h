/*
 * GUINcurses.h
 *
 * NCURSES based GUI for the system
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef GUINCURSES_H_
#define GUINCURSES_H_

#include "../GUI.h"
#include <vector>
#include <cstdint>
#include <mutex>


// forward declarations
typedef struct _win_st WINDOW;
class PnlGlobal;
class PnlHttp;
class PnlHttpDetails;
class GWindow;
class Thread;

class GUINcurses : public GUI {

public:

	/** singleton access */
	static GUINcurses* get();

	/** dtor */
	virtual ~GUINcurses();

	void update() override;

	void setModel(const Summary& sum) override;

	/** set the gui's update interval in milliseconds */
	void setUpdateInterval(uint32_t intervalMs);

protected:

	friend class GWindow;

	/** windows will call this method to attach them to the visible windows */
	void attach(GWindow* panel);

	/** windows will call this method to detach them fromt the visible windows */
	void detach(GWindow* panel);

private:



	/** ctor */
	GUINcurses();


	/** key event occurred */
	void onKey(int key);

	/** called when terminal-window is resized */
	void onResize();

	/** static handler for the resize signal */
	static void resizeHandler(int sig);


	/** store all panels here */
	std::vector<GWindow*> windows;

	/** the panel that currently has the focus */
	uint32_t focusedIdx;

	/** the thread to track key inputs */
	Thread* keyThread;

	/** reference to the data-model */
	const Summary* summary;

	/** will be set to true (once) when panels are added */
	bool needsResize;

	/** synchronize some methods */
	std::mutex mutex;

	/** the update interval of the gui */
	uint32_t updateInterval;

	/** the thread to update the gui */
	Thread* updateThread;

};

#endif /* GUINCURSES_H_ */
