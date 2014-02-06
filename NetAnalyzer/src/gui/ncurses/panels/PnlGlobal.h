/*
 * PnlGlobal.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef PNLGLOBAL_H_
#define PNLGLOBAL_H_

#include "../elements/GPanel.h"

// forward declarations
struct Summary;



class PnlGlobal : public GPanel {

public:

	/** ctor */
	PnlGlobal(const Summary& sum);

	/** dtor */
	virtual ~PnlGlobal();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

private:

	/** the summary to display */
	const Summary& sum;

};

#endif /* PNLGLOBAL_H_ */
