/*
 * GPanel.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef GWINDOW_H_
#define GWINDOW_H_

#include <string>

#ifdef __CYGWIN__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "GColors.h"
#include "../GUINcurses.h"
#include "GKeyListener.h"
#include "GPanel.h"
#include "GRender.h"

// forward declarations
typedef struct _win_st WINDOW;
struct Summary;


/**
 * represents one window that can be added to the console GUI
 */
class GWindow : public GKeyListener {

public:

	//	/** ctor */
	//	GWindow(const std::string& title, int w, int h, int x, int y) :
	//		title(title), hasFocus(false), width(w), height(h) {
	//		win = newwin(h, w, y, x);
	//		GUINcurses::get()->attach(this);
	//	}

	/** ctor */
	GWindow(const std::string& title, GPanel& pnl) :
		render(this), pnl(pnl), title(title), hasFocus(false), width(0), height(0) {
		win = newwin(0,0,0,0);
		GUINcurses::get()->attach(this);
	}

	/** dtor */
	virtual ~GWindow() {
		if (win != nullptr) {delwin(win); win = nullptr;}
		GUINcurses::get()->detach(this);
	}

	/** a key event occured for this panel */
	bool onKey(int key) {
		return pnl.onKey(key);
	}

	/**
	 * called when terminal window is resized.
	 * each panel must set it's size and position here using setBounds()!
	 */
	virtual void onResize() = 0;

	/** refresh this window */
	void refresh() {

		// when focused, change background color
		// this color will be used for werase()
		wbkgd(win, COLOR_PAIR( hasFocus ? CIDX_WIN_FOCUSED : CIDX_WIN_UNFOCUSED ));

		// clear the window's content
		werase(win);

		// draw a border with title
		wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
		mvwprintw(win, 0, 2, title.c_str());

		// now set the color for the text-content of the window
		wattron(win, COLOR_PAIR( CIDX_WIN_CONTENT ));

		// update the content
		pnl.render(render);

		// queue changes (flush all window changes with doupdate())
		wnoutrefresh(win);

	}

	/** get the window this panel belongs to */
	WINDOW* getWindow() const {return win;}

	/** make this panel focused */
	void setFocus(bool hasFocus) {this->hasFocus = hasFocus;}

	/** resize/move this panel */
	void setBounds(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
		width = w;
		height = h;
		wresize(win, h, w);
		mvwin(win, y, x);
		pnl.setBounds(1,1,w-2,h-2);
	}

protected:

	/** the renderer to render to this window */
	GRender render;

	/** the panel to display */
	GPanel& pnl;

	/** the title of this panel */
	std::string title;

	/** has this panel currently the focus? */
	bool hasFocus;

	/** the ncurses window to use for this panel */
	WINDOW* win;

	/** the window's width */
	uint32_t width;

	/** the window's height */
	uint32_t height;



};


#endif /* GWINDOW_H_ */
