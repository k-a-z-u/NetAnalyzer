/*
 * GuiHelper.h
 *
 *  Created on: Apr 25, 2013
 *      Author: kazu
 */

#ifndef GUIHELPER_H_
#define GUIHELPER_H_

#include <string>
#include <cstdint>

// forward declarations
class GRender;

#define GH_TYPE_BYTE		0
#define GH_TYPE_KBYTE		1
#define GH_TYPE_MBYTE		2

namespace GuiHelper {

/** print (right-aligned) int with percentage value */
void printIntPercent(GRender& render, uint32_t x, uint32_t y, long total, long other, const std::string& unit);

/** print (right-aligned) int with percentage value using a unit as bytes / kbytes / .. (see toggleBytePercentUnit) */
void printBytesPercent(GRender& render, uint32_t x, uint32_t y, long total, long other);

/** toggle the unit used for printBytesPercent */
void toggleBytePercentUnit();

/** print (right-aligned) int */
void printInt(GRender& render, uint32_t x, uint32_t y, long val);

/** print (right-aligned) float */
void printFloat(GRender& render, uint32_t x, uint32_t y, float val, const std::string& unit);

extern int bytePercentUnit;

}

#endif /* GUIHELPER_H_ */
