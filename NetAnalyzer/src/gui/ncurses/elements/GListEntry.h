/*
 * GListEntry.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef GLISTENTRY_H_
#define GLISTENTRY_H_

/**
 * one entry within the GList
 */
class GListEntry {

public:

	/** dtor */
	virtual ~GListEntry() {;}

	/** get the value at the given column */
	virtual std::string getValue(uint32_t column) = 0;

};


#endif /* GLISTENTRY_H_ */
