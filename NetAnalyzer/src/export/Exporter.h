/*
 * Exporter.h
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifndef EXPORTER_H_
#define EXPORTER_H_


// forward declarations
struct Summary;
class NetSource;
class File;


/**
 * interface for all exporters that work on a Summary
 */
class Exporter {

public:

	/** ctor */
	Exporter();

	/** dtor */
	virtual ~Exporter() {;}

	/** set the data source for the exporter */
	void setModel(const Summary& sum);


	/** set the folder all exporters write to */
	static void setFolder(File file);

protected:

	friend class NetSource;

	/** called when the exporter is added to the Analyzer */
	virtual void visit(NetSource& src) = 0;

	/** the summary to work on */
	const Summary* sum;

	/** the folder all exporters write to */
	static File folder;


};


#endif /* EXPORTER_H_ */
