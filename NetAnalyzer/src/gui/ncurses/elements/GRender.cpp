/*
 * GRender.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#include "GRender.h"
#include "GWindow.h"
#include "../../../Helper.h"

#ifdef __CYGWIN__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif



GRender::GRender(GWindow* win) : win(win), borderSize(0) {;}

void GRender::printChar(uint32_t sx, uint32_t sy, unsigned long c) {
	if (sx >= b.w - borderSize) {return;}
	if (sy >= b.h - borderSize) {return;}
	wmove(win->getWindow(), borderSize + b.y + sy, borderSize + b.x + sx);
	waddch(win->getWindow(), c);
}

void GRender::print(uint32_t sx, uint32_t sy, const std::string& str) {
	if (b.w <= borderSize * 2 + sx) {return;}
	if (b.h <= borderSize * 2 + sy) {return;}
	uint32_t max = b.w - borderSize*2 - sx;
	if (str.length() > max) {
		mvwprintw(win->getWindow(), borderSize + b.y + sy, borderSize + b.x + sx, str.substr(0, max).c_str());
	} else {
		mvwprintw(win->getWindow(), borderSize + b.y + sy, borderSize + b.x + sx, str.c_str());
	}
}

void GRender::printR(uint32_t sx, uint32_t sy, const std::string& str) {
	print (sx-str.length(), sy, str);
}

void GRender::drawVLine(uint32_t sx, uint32_t sy, uint32_t h, uint32_t c) {
	mvwvline(win->getWindow(), borderSize + b.y + sy, borderSize + b.x + sx, c, h - borderSize * 2);
}

void GRender::drawHLine(uint32_t sx, uint32_t sy, uint32_t w, uint32_t c) {
	mvwhline(win->getWindow(), borderSize + b.y + sy, borderSize + b.x + sx, c, w - borderSize * 2);
}

void GRender::drawBox(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {

	--w;
	--h;

	// draw edges
	printChar(x,	y,		ACS_ULCORNER);
	printChar(x+w,	y,		ACS_URCORNER);
	printChar(x,	y+h,	ACS_LLCORNER);
	printChar(x+w,	y+h,	ACS_LRCORNER);

	// draw lines
	drawHLine(x+1,	y,		w-1, ACS_HLINE);
	drawHLine(x+1,	y+h,	w-1, ACS_HLINE);
	drawVLine(x,	y+1,	h-1, ACS_VLINE);
	drawVLine(x+w,	y+1,	h-1, ACS_VLINE);

}

void GRender::setColors(uint32_t idx) {
	wattron(win->getWindow(), COLOR_PAIR( idx ));
}

void GRender::setBounds(const GBounds& bounds) {
	b.set(bounds);
	this->borderSize = 0;
}

void GRender::setBorderSize(uint32_t borderSize) {
	this->borderSize = borderSize;
}


