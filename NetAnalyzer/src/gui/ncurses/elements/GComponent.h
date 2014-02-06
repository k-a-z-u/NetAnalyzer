/*
 * GComponent.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef GCOMPONENT_H_
#define GCOMPONENT_H_

#include "GKeyListener.h"
#include "GRender.h"

#include <cstdint>

class GComponent : public GKeyListener {

public:

	/** dtor */
	virtual ~GComponent() {;}

	/** every GComponent must draw it's content here */
	virtual void refresh(GRender& render) = 0;

	/** set the bounds of this component */
	virtual void setBounds(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

};

#endif /* GCOMPONENT_H_ */
