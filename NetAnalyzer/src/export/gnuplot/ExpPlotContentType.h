/*
 * ExpPlotContentType.h
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifndef EXPPLOTCONTENTTYPE_H_
#define EXPPLOTCONTENTTYPE_H_

#include "../PeriodicExporter.h"
#include <map>
#include <fstream>

#define PLOT_MAX_COLUMS	32

/**
 * export content-type distribution (which content-type consumes how many % of traffic)
 *
 * format:
 *	type1%	type2%	type3%	type4%	...
 *
 */
class ExpPlotContentType : public PeriodicExporter {

public:

	/** ctor */
	ExpPlotContentType(PeriodicExporterThread* thread);

	/** dtor */
	virtual ~ExpPlotContentType();

	/**
	 *  add one content-type to export stats for.
	 *  it's possible to map several content-types to the same column!
	 *  (e.g. text/javascript and application/javascript)
	 */
	void addContentType(const std::string& contentType, uint32_t column);

private:

	/** hidden copy ctor */
	ExpPlotContentType(const ExpPlotContentType&);

	void exportMe() override;

	/** map each content-type to a column of the output file here */
	std::map<std::string, int> contentTypes;

	/** the file to write to */
	std::ofstream stream;

	/** first export? */
	bool first;

	/** the number of columns to export (+1 for "other") */
	uint32_t numColumns;

};

#endif /* EXPPLOTCONTENTTYPE_H_ */
