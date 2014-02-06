/*
 * convHex.h
 *
 *  Created on: Apr 28, 2013
 *      Author: kazu
 */

#ifndef CONVHEX_H_
#define CONVHEX_H_

#include <sstream>


/** convert hexadecimal string to int */
static int32_t hexStringToInt(const std::string& str) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	int32_t ret = 0;
	ss << std::hex << str;
	ss >> ret;
	return ret;
}


#endif /* CONVHEX_H_ */
