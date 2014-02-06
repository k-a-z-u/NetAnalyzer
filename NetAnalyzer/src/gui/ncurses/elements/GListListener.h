/*
 * GListListener.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef GLISTLISTENER_H_
#define GLISTLISTENER_H_

/**
 * listen for events within GList
 */
class GListListener {

public:

	/** dtor */
	virtual ~GListListener() {;}

	/** the given entry has been selected */
	virtual void onSelectedItem(GListEntry* entry) = 0;

};


#endif /* GLISTLISTENER_H_ */
