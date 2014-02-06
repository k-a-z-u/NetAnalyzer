/*
 * GPanel.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef GPANEL_H_
#define GPANEL_H_


#include "GComponent.h"
#include "GBounds.h"

/**
 * this is a logical unit one can draw to
 */
class GPanel : public GComponent {

public:

	/** ctor */
	GPanel() : b() {;}

	/** dtor */
	virtual ~GPanel() {;}

	/** set the bounds (position/size) of this GPanel */
	void setBounds(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override {
		b.set(x,y,w,h);
	}

	/** get the bounds (position/size) of this GPanel */
	const GBounds& getBounds() {return b;}

	/** trigger refresh */
	void render(GRender& render) {
		render.setBounds(b);
		refresh(render);
	}

protected:

	/** the bounds of this panel */
	struct GBounds b;

};


#endif /* GPANEL_H_ */
