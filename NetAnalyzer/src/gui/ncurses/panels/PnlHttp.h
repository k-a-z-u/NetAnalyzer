/*
 * PnlHttp.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef PNLHTTP_H_
#define PNLHTTP_H_



#include "../elements/GPanel.h"

// forward declarations
struct Summary;


class PnlHttp : public GPanel {

public:

	/** ctor */
	PnlHttp(const Summary& sum);

	/** dtor */
	virtual ~PnlHttp();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

private:

	/** the summary to display */
	const Summary& sum;

};
#endif /* PNLHTTP_H_ */
