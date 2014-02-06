/*
 * PnlHttpDetails.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef PNLHTTPDETAILS_H_
#define PNLHTTPDETAILS_H_

#include "../elements/GPanel.h"
#include "../elements/GList.h"

// forward declarations
struct Summary;

class PnlHttpDetails : public GPanel, public GListListener {

public:

	/** ctor */
	PnlHttpDetails(const Summary& sum);

	/** dtor */
	virtual ~PnlHttpDetails();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

	void onSelectedItem(GListEntry* entry) override;

private:

	/** the summary to display */
	const Summary& sum;

	/** list for all content-types */
	GList list;

};

#endif /* PNLHTTPDETAILS_H_ */
