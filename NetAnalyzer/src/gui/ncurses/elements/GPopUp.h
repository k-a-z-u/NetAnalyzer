/*
 * GPopUp.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef GPOPUP_H_
#define GPOPUP_H_

#include "GWindow.h"
#include "GPanel.h"

class GPopUp : public GWindow {

public:

	/** ctor */
	GPopUp(const std::string& title, GPanel& pnl) : GWindow(title, pnl) {
		;
	}

	void onResize() override {
		setBounds(2, 2, COLS-4,LINES-4);
	}

	bool onKey(int key) override {

		// ESC kills the popup
		if (key == 27) {delete this; return true;}

		// pass key to pop-up's content
		pnl.onKey(key);

		// popup consumes all keys!
		// no keys get bubble to lower elements
		return true;

	}

};


#endif /* GPOPUP_H_ */
