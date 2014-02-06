/*
 * HTTPDataSource.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#ifndef HTTPDATASOURCE_H_
#define HTTPDATASOURCE_H_

//forward declarations
class Analyzer;
class Exporter;

/**
 * a HTTPDataSource is an object that will trigger
 * onHttpConnection() within the Analyzer whenever a new
 * "http-connection" has been established
 */
class HTTPDataSource {

public:

	/** dtor */
	virtual ~HTTPDataSource() {;}

	/**
	 * called from Analyzer when this data-source is added to it
	 */
	virtual void accept(Analyzer& analyzer) = 0;

	/**
	 * called from Analyzer when a new Exporter is added.
	 * the DataSource MAY call exp->visit(this)
	 */
	virtual void accept(Exporter& exp) = 0;

	/** start providing data */
	virtual void start() = 0;

	/** stop providing data */
	virtual void stop() = 0;

};

#endif /* HTTPDATASOURCE_H_ */
