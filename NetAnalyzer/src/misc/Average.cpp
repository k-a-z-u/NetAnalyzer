/*
 * Average.cpp
 *
 *  Created on: Apr 20, 2013
 *      Author: kazu
 */

#include "Average.h"

Average::Average() : sum(0), numVals(0) {
	// TODO Auto-generated constructor stub

}

Average::~Average() {
	// TODO Auto-generated destructor stub
}


void Average::add(uint32_t val) {
	++numVals;
	sum += val;
}

/** get the current average */
uint32_t Average::getAverage() const {
	if (numVals == 0) {return 0;}
	return sum / numVals;
}
