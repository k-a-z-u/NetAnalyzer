/*
 * Statistics.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <set>
#include <cstdint>

/**
 * store values here and get statistics about their
 * avg, median, std-dev, etc.
 */
class Statistics {

public:

	/** ctor */
	Statistics();

	/** dtor */
	virtual ~Statistics();

	/** add a new value */
	void add(uint32_t val);

	/** get the std-dev of all values */
	float getStdDev() const;

	/** get average value */
	float getAvg() const;

	/** get the given quantile (e.g. 0.5 for median) */
	float getQuantile(float q) const;

	/** get the median value (= Quantile 0.5) */
	float getMedian() const;

	/** get smallest value */
	float getMin() const;

	/** get largest value */
	float getMax() const;

	/** get the sum of all values */
	uint64_t getSum() const;

	/** get number of stored values */
	uint32_t getCount() const;

	/** get the number of values that are below "val" */
	uint32_t getNumValuesBelow(uint32_t val) const;

private:

	/** sum of all added values */
	uint64_t sum;

	/** squared sum of all added values (for std-dev) */
	uint64_t sumSquared;

	/** the number of added values */
	uint32_t cnt;

	/** multiset to sort all values */
	std::multiset<uint32_t> values;

};


#endif /* STATISTICS_H_ */
