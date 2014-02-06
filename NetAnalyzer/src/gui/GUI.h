/*
 * GUI.h
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef GUI_H_
#define GUI_H_

struct Summary;


class GUI {

public:

	/** dtor */
	virtual ~GUI() {;}

	/** trigger data update within the gui */
	virtual void update() = 0;

	/** set the data model for the gui to display */
	virtual void setModel(const Summary& sum) = 0;


};

#endif /* GUI_H_ */
