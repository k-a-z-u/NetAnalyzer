/*
 * GBounds.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef GBOUNDS_H_
#define GBOUNDS_H_

#include <cstdint>

struct GBounds {

	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;

	/** ctor */
	GBounds() : x(0), y(0), w(0), h(0) {;}

	/** set bounds */
	void set(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	/** set bounds */
	void set(const GBounds& b) {
		this->x = b.x;
		this->y = b.y;
		this->w = b.w;
		this->h = b.h;
	}

};

#endif /* GBOUNDS_H_ */
