/*
 * GRender.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef GRENDER_H_
#define GRENDER_H_


#include <string>

#include "GBounds.h"

// forward declarations
class GWindow;


class GRender {

public:

	/** ctor */
	GRender(GWindow* win);

	/** print one char to (x,y) within this panel. if the char is outside the panel it won't be printed! */
	void printChar(uint32_t sx, uint32_t sy, unsigned long c);

	/** convenience function to print text. text outside of the panels region won't be printed */
	void print(uint32_t sx, uint32_t sy, const std::string& str);

	/** convenience function to print right-aligned text. text outside of the panels region won't be printed */
	void printR(uint32_t sx, uint32_t sy, const std::string& str);

	/** draw vertical line */
	void drawVLine(uint32_t sx, uint32_t sy, uint32_t h, uint32_t c);

	/** draw horizontal line */
	void drawHLine(uint32_t sx, uint32_t sy, uint32_t w, uint32_t c);

	/** draw a box within this panel */
	void drawBox(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

	/** set the colors for drawing */
	void setColors(uint32_t idx);


	/** set the bounds (position/size) to render to */
	void setBounds(const GBounds& bounds);

	/** set insets */
	void setBorderSize(uint32_t borderSize);

private:

	/** the window to render to */
	GWindow* win;

	/** the region to render to */
	GBounds b;

	uint32_t borderSize;

};

#endif /* GRENDER_H_ */
