/*
 * GTabHost.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef GTABHOST_H_
#define GTABHOST_H_

#ifdef __CYGWIN__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <vector>

#include "GKeyListener.h"
#include "GPanel.h"
#include "GTab.h"
#include "GColors.h"

/**
 * contains several tabs and can be rendered
 */
class GTabHost : public GPanel {


public:

	/** ctor */
	GTabHost() : selIdx(0) {
		;
	}

	/** dtor */
	~GTabHost() {

		// delete all tabs
		for (auto tab : tabs) {delete tab;}

	}


	/** key pressed */
	bool onKey(int key) override {

		// react to TAB-key
		if (key == '\t' || key == KEY_RIGHT) {
			if (tabs.empty()) {selIdx = 0; return true;}
			++selIdx %= tabs.size();
			return true;
		} else if (key == KEY_LEFT) {
			if (tabs.empty()) {selIdx = 0; return true;}
			if (selIdx == 0) {selIdx = tabs.size() -1;} else {--selIdx;}
			return true;
		}

		// bubble to childs
		if (tabs.empty()) {return false;}
		return tabs[selIdx]->onKey(key);

	}

	/** add a new tab to this host */
	void addTab(GTab* tab) {
		tabs.push_back(tab);
	}

	/** remove all tabs from this host */
	void removeTabs() {
		for (auto& tab : tabs) {
			delete tab;
		}
		tabs.clear();
	}

	/** redraw the content */
	void refresh(GRender& render) override {

		// draw box for content
		render.drawBox(0, 2, b.w, b.h-2);

		// draw all tabs
		uint32_t curX = 1;
		for (uint32_t idx = 0; idx < tabs.size(); ++idx) {

			// is this tab selected?
			render.setColors( (selIdx == idx) ? (CIDX_TAB_SELECTED) : (CIDX_WIN_CONTENT) );

			// draw border + title
			render.drawBox(curX, 0, tabs[idx]->getTitle().length()+2, 3);
			render.print(curX+1, 1, tabs[idx]->getTitle());
			curX += tabs[idx]->getTitle().length() + 2;

		}

		// normal color
		render.setColors(CIDX_WIN_CONTENT);

		// draw current tab's content
		tabs[selIdx]->setBounds(2, 4, b.w - 4, b.h - 4);
		tabs[selIdx]->render(render);

	}

private:

	/** the currently selected tab */
	uint32_t selIdx;

	/** store all tabs here */
	std::vector<GTab*> tabs;

};

#endif /* GTABHOST_H_ */
