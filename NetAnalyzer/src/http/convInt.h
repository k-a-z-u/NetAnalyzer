/*
 * convInt.h
 *
 *  Created on: Apr 28, 2013
 *      Author: kazu
 */

#ifndef CONVINT_H_
#define CONVINT_H_

/** convert string to int */
static int32_t strToInt(const std::string& str) {
	return atoi(str.c_str());
}



#endif /* CONVINT_H_ */
