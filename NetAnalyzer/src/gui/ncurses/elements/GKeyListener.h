/*
 * GKeyListener.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef GKEYLISTENER_H_
#define GKEYLISTENER_H_


/**
 * interface for all key-input-listeners
 */
class GKeyListener {

public:

	/** dtor */
	virtual ~GKeyListener() {;}

	/**
	 * called whenever a key-input is received.
	 * if the receiver returns true, it consumed the key,
	 * and the event won't be bubbled
	 */
	virtual bool onKey(int key) = 0;

};

#endif /* GKEYLISTENER_H_ */
