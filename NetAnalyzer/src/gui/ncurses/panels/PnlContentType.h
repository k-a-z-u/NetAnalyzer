/*
 * PnlContentType.h
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#ifndef PNLCONTENTTYPE_H_
#define PNLCONTENTTYPE_H_

#include "../elements/GPanel.h"
#include "../elements/GTabHost.h"

// forward declaration
struct HTTPContentType;


class PnlContentType : public GPanel {

public:

	/** ctor */
	PnlContentType();

	/** dtor */
	virtual ~PnlContentType();

	bool onKey(int key) override;

	void refresh(GRender& render) override;

	/** set the content type to display */
	void setContentType(const HTTPContentType& type);

	/** get the content-type this panel is for */
	const HTTPContentType& getContentType();

private:

	void recreateTabs();

	/** the http content type to display */
	const HTTPContentType* type;

	/** store tabs here */
	GTabHost tabs;

};


#endif /* PNLCONTENTTYPE_H_ */
