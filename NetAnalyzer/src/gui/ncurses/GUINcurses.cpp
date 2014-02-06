/*

 * GUINcurses.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#include "GUINcurses.h"

#ifdef __CYGWIN__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <unistd.h>

#include "../../Helper.h"
#include "../../analyzer/Summary.h"

#include "elements/GWindow.h"
#include "panels/PnlGlobal.h"
#include "panels/PnlHttp.h"
#include "panels/PnlHttpDetails.h"
#include "panels/PnlHelp.h"
#include "panels/PnlTcp.h"

#include "../../misc/Thread.h"
#include "elements/GColors.h"

#include "../../quit.h"
#include <csignal>


GUINcurses* GUINcurses::get() {
	static GUINcurses* INSTANCE = new GUINcurses();
	return INSTANCE;
}

GUINcurses::GUINcurses() : focusedIdx(0), summary(nullptr), needsResize(false), updateInterval(500) {

	// call method on terminal window resize
	signal(SIGWINCH, &GUINcurses::resizeHandler);

	// start ncurses mode
	initscr();

	// enable color mode
	start_color();
	//use_default_colors();

	// configure colors
	//init_pair(CIDX_WIN_UNFOCUSED,	COLOR_WHITE,	COLOR_BLACK);
	//init_pair(CIDX_WIN_FOCUSED,		COLOR_YELLOW,	COLOR_BLACK);
	//init_pair(CIDX_WIN_CONTENT,		COLOR_WHITE,	COLOR_BLACK);
	//init_pair(CIDX_LST_HEADER,		COLOR_BLACK,	COLOR_WHITE);
	//init_pair(CIDX_LST_FOCUSED,		COLOR_WHITE,	COLOR_BLUE);

	init_pair(CIDX_WIN_UNFOCUSED,	COLOR_BLACK,	COLOR_WHITE);
	init_pair(CIDX_WIN_FOCUSED,		COLOR_BLUE,		COLOR_WHITE);
	init_pair(CIDX_WIN_CONTENT,		COLOR_BLACK,	COLOR_WHITE);
	init_pair(CIDX_LST_HEADER,		COLOR_WHITE,	COLOR_BLACK);
	init_pair(CIDX_LST_FOCUSED,		COLOR_WHITE,	COLOR_BLUE);
	init_pair(CIDX_TAB_SELECTED,	COLOR_BLUE,		COLOR_WHITE);


	// fill with default background??
	//bkgd(COLOR_PAIR(CIDX_WIN_UNFOCUSED));

	// default: block 1 second after ESC-key
	set_escdelay(25);

	// enable special keys for input (key_up, key_down, ..)
	keypad(stdscr, TRUE);

	// the thread to listen for key inputs
	class KeyThread : public Thread {
	public:
		GUINcurses* gui;
		KeyThread(GUINcurses* gui) : gui(gui) {;}
		void run() {
			while(enabled) {gui->onKey(getch());}
		}
	};

	// create and start the key thread
	keyThread = new KeyThread(this);
	keyThread->start();

	// create the update thread
	struct UpdateThread : Thread {
		GUINcurses* parent;
		UpdateThread(GUINcurses* parent) : parent(parent) {;}
		void run() {
			while(enabled) {
				parent->update();
				usleep(parent->updateInterval * 1000);
			}
		}
	};

	updateThread = new UpdateThread(this);
	updateThread->start();

}

GUINcurses::~GUINcurses() {

	// stop the threads
	if (keyThread != nullptr)		{delete keyThread; keyThread = nullptr;}
	if (updateThread != nullptr)	{delete updateThread; updateThread = nullptr;}

	// remove all panels
	// this works because GPanel's dtor will detach them from
	// the panels vector!
	while (!windows.empty()) {
		delete windows[0];
	}

	// end ncurses mode
	endwin();

}

void GUINcurses::setUpdateInterval(uint32_t intervalMs) {
	updateInterval = intervalMs;
}

void GUINcurses::resizeHandler(int sig) {

	(void) sig;

	static bool isResizing = false;

	// prevent multiple calls at the same time
	// as this would otherwise lock the system!
	if (isResizing) {return;}
	isResizing = true;
	GUINcurses::get()->onResize();
	isResizing = false;
}

#include <sys/ioctl.h>


void GUINcurses::onResize() {
	mutex.lock();

	// get new terminal size
	struct winsize size;
	if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
		resize_term(size.ws_row, size.ws_col);
		wrefresh(curscr);
	}

	// resize all windows
	for (auto it : windows) {it->onResize();}

	mutex.unlock();
}

void GUINcurses::attach(GWindow* pnl) {

	// add panel to panels list and focus it
	windows.push_back(pnl);
	focusedIdx = windows.size() - 1;

	// next call to refresh will resize all window including the new panel
	// refreshing the new panel here will crash as the panel will add itself
	// during it's construction phase -> not all virtual methods established
	needsResize = true;

}

void GUINcurses::detach(GWindow* pnl) {

	// get the index of the detached panel to adjust the focus index
	uint32_t index = &pnl - &windows[0];
	if (index >= focusedIdx) {--focusedIdx;}

	// remove panel from panels list
	windows.erase(std::remove(windows.begin(), windows.end(), pnl), windows.end());

}


void GUINcurses::onKey(int key) {

	// sanity check
	if (windows.empty()) {return;}

	// pass the key to the focused window. if the window consumes this key -> stop
	if (!windows[focusedIdx]->onKey(key)) {

		// react
		switch (key) {
		case KEY_F(2):	quit(); return;
		case '\t':		++focusedIdx; focusedIdx %= windows.size(); break;
		}

	}

	// update all windows
	update();

}

void GUINcurses::update() {
	mutex.lock();

	// resize needed?
	// (return because: onResize() will call update()
	if (needsResize) {
		needsResize = false;
		mutex.unlock();
		onResize();
		return;
	}

	// refresh all panels
	for (size_t i = 0; i < windows.size(); ++i) {
		windows[i]->setFocus(i == focusedIdx);
		windows[i]->refresh();
	}

	// flush changes
	doupdate();

	mutex.unlock();
}

void GUINcurses::setModel(const Summary& sum) {

	summary = &sum;

	class WinGlobal : public GWindow {
		public: WinGlobal(GPanel& pnl) : GWindow("Global", pnl) {;}
		void onResize() {setBounds(0,0,38,9);}
	};
	class WinHelp : public GWindow {
		public: WinHelp(GPanel& pnl) : GWindow("Help", pnl) {;}
		void onResize() {setBounds(103,0, COLS-103, 9);}
	};
	class WinHTTP : public GWindow {
		public: WinHTTP(GPanel& pnl) : GWindow("HTTP", pnl) {;}
		void onResize() {setBounds(65,0, 38,9);}
	};
	class WinTCP : public GWindow {
		public: WinTCP(GPanel& pnl) : GWindow("TCP-Processor", pnl) {;}
		void onResize() {setBounds(38,0, 27,9);}
	};
	class WinHttpDetails : public GWindow {
		public: WinHttpDetails(GPanel& pnl) : GWindow("HTTP-Server-Response-Details", pnl) {;}
		void onResize() {setBounds(0,9, COLS,LINES-9);}
	};


	static PnlGlobal pnlGlobal(sum);
	static PnlTcp pnlTcp(sum);
	static PnlHttp pnlHttp(sum);
	static PnlHelp pnlHelp;
	static PnlHttpDetails pnlHttpDet(sum);

	// init the windows
	// they will attach themselves to the windows-vector!
	new WinGlobal( pnlGlobal );
	new WinTCP( pnlTcp );
	new WinHTTP( pnlHttp );
	new WinHelp( pnlHelp );
	new WinHttpDetails( pnlHttpDet );

	onResize();

}
