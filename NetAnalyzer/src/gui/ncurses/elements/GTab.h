/*
 * GTab.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef GTAB_H_
#define GTAB_H_

#include <string>

#include "GKeyListener.h"
#include "GPanel.h"

/**
 * one Tab within a TabHost
 */
class GTab : public GPanel {

public:

	/** ctor */
	GTab(const std::string& title) : title(title) {
		;
	}

	/** dtor */
	~GTab() {
		;
	}

	/** get the title of this tab to display in the tabhost */
	const std::string getTitle() const {
		return title;
	}

private:

	/** this tab's title */
	std::string title;

};

#endif /* GTAB_H_ */
