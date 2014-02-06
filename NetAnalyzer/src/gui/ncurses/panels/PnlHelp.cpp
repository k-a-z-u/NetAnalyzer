/*
 * PnlHelp.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#include "PnlHelp.h"


PnlHelp::PnlHelp() : GPanel() {
}

PnlHelp::~PnlHelp() {
	;
}

bool PnlHelp::onKey(int key) {
	(void) key;
	return false;
}

void PnlHelp::refresh(GRender& render) {

	render.print(1, 0, "F2:       exit");
	render.print(1, 1, "TAB:      switch windows");
	render.print(1, 2, "UP/DOWN:  navigate within list");
	render.print(1, 3, "ENTER:    select entry");
	render.print(1, 4, "H:        change B/KiB/MiB");
	render.print(1, 5, "ESC:      close popup windows");

}
