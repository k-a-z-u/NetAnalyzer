/*
 * Statistics.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#include <cmath>

#include "Statistics.h"


Statistics::Statistics() : sum(0), sumSquared(0), cnt(0) {
	;
}

Statistics::~Statistics() {
	;
}

void Statistics::add(uint32_t value) {
	++cnt;
	sum += value;
	sumSquared += (value*value);
	values.insert(value);
}

float Statistics::getStdDev() const {
	float E1 = sumSquared / (float) cnt;
	float E2 = getAvg();
	return sqrt(E1 - (E2*E2));
}

float Statistics::getAvg() const {
	return sum / (float) cnt;
}

#include <iostream>
float Statistics::getQuantile(float q) const {
	if (q < 0 || q >= 1.0) {return -1;}
	uint32_t pos = cnt * q;
	uint32_t curPos = 0;
	for (auto val : values) {
		if (curPos == pos) {return val;}
		++curPos;
	}
	return -1;
}

float Statistics::getMedian() const {
	return getQuantile(0.5f);
}

float Statistics::getMin() const {
	if (values.empty()) {return -1;}
	return *(values.begin());
}

float Statistics::getMax() const {
	if (values.empty()) {return -1;}
	return *(--values.end());
}

uint64_t Statistics::getSum() const {
	return sum;
}

uint32_t Statistics::getCount() const {
	return cnt;
}

uint32_t Statistics::getNumValuesBelow(uint32_t val) const {
	uint32_t numFound = 0;
	for (auto curVal : values) {
		if (curVal > val) {return numFound;}
		++numFound;
	}
	return numFound;
}
