/*
 * PnlTcp.h
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#ifndef PNLTCP_H_
#define PNLTCP_H_



#include "../elements/GPanel.h"

// forward declarations
struct Summary;

class PnlTcp : public GPanel {

public:

	/** ctor */
	PnlTcp(const Summary& sum);

	/** dtor */
	virtual ~PnlTcp();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

private:

	/** the summary to display */
	const Summary& sum;

};

#endif /* PNLTCP_H_ */
