/*
 * GUIDummy.h
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef GUIDUMMY_H_
#define GUIDUMMY_H_

#include "GUI.h"


/**
 * dummy gui that does nothing
 */
class GUIDummy : public GUI {

public:

	/** ctor */
	GUIDummy() {;}

	/** dtor */
	~GUIDummy() {;}

	void update() override {;}

	void setModel(const Summary& sum) override {
		(void) sum;
	}

};


#endif /* GUIDUMMY_H_ */
