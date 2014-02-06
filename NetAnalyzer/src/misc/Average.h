/*
 * Average.h
 *
 *  Created on: Apr 20, 2013
 *      Author: kazu
 */

#ifndef AVERAGE_H_
#define AVERAGE_H_

#include <cstdint>

class Average {

public:

	/** ctor */
	Average();

	/** dtor */
	virtual ~Average();

	/** add one new value to the averager */
	void add(uint32_t val);

	/** get the current average */
	uint32_t getAverage() const;

private:

	/** the current sum of all averages */
	uint64_t sum;

	/** the number of added values */
	uint32_t numVals;

};

#endif /* AVERAGE_H_ */
