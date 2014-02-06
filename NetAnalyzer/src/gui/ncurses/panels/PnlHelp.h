/*
 * PnlHelp.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef PNLHELP_H_
#define PNLHELP_H_

#include "../elements/GPanel.h"

class PnlHelp : public GPanel {

public:

	/** ctor */
	PnlHelp();

	/** dtor */
	virtual ~PnlHelp();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

};

#endif /* PNLHELP_H_ */
