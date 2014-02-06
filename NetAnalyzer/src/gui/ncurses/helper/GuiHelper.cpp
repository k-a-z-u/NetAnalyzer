/*
 * GuiHelper.cpp
 *
 *  Created on: Apr 25, 2013
 *      Author: kazu
 */

#include "GuiHelper.h"

#include "../elements/GRender.h"
#include <sstream>
#include <iomanip>

int GuiHelper::bytePercentUnit = GH_TYPE_BYTE;

/** toggle the unit used for byte percent */
void GuiHelper::toggleBytePercentUnit() {
	++GuiHelper::bytePercentUnit;
	GuiHelper::bytePercentUnit %= (GH_TYPE_MBYTE+1);
}

void GuiHelper::printBytesPercent(GRender& render, uint32_t x, uint32_t y, long total, long other) {

	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss.imbue(std::locale(""));

	std::string unit;
	int div = 1;
	int digits = 0;

	switch (bytePercentUnit) {
	case GH_TYPE_BYTE:	div = 1; digits = 0; unit = " B"; break;
	case GH_TYPE_KBYTE: div = 1024; digits = 1; unit = " KiB"; break;
	case GH_TYPE_MBYTE: div = 1024 * 1024; digits = 1; unit = " MiB"; break;
	}

	float percent = (total) ? (100.0 * other / (total)) : (0);
	ss << " (" << std::setiosflags(std::ios::fixed) << std::setprecision(1) << percent << "%)";
	render.printR(x, y, ss.str());
	ss.str("");
	ss << std::setprecision(digits) << (other / (float) div) << unit ;
	render.printR(x-9, y, ss.str());

}

void GuiHelper::printIntPercent(GRender& render, uint32_t x, uint32_t y, long total, long other, const std::string& unit) {

	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss.imbue(std::locale(""));

	float percent = (total) ? (100.0 * other / (total)) : (0);
	ss << " (" << std::setiosflags(std::ios::fixed) << std::setprecision(1) << percent << "%)";
	render.printR(x, y, ss.str());
	ss.str("");
	ss << other << unit ;
	render.printR(x-9, y, ss.str());

}

void GuiHelper::printInt(GRender& render, uint32_t x, uint32_t y, long val) {

	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss.imbue(std::locale(""));

	ss << val;
	render.printR(x, y, ss.str());

}

void GuiHelper::printFloat(GRender& render, uint32_t x, uint32_t y, float val, const std::string& unit) {

	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << std::setiosflags(std::ios::fixed);
	ss << std::setprecision(1);

	ss << val << unit;
	render.printR(x, y, ss.str());

}
