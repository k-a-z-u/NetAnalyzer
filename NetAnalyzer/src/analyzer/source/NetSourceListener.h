/*
 * NetSourceListener.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef NETSOURCELISTENER_H_
#define NETSOURCELISTENER_H_

// forward declaration
class HTTPDecoder;
class HTTPAnalyzer;

/**
 * register for callbacks within the NetSource.
 * This allows e.g. exporters to register themselves within the created HTTP-decoder
 * to export it's stats.
 */
class NetSourceListener {

public:

	/** dtor */
	virtual ~NetSourceListener() {;}

	/** a new TCP-connection has been open and a HTTPDecoder was created */
	virtual void onNewTcpConnection(HTTPAnalyzer& analyzer, HTTPDecoder& dec) = 0;

};


#endif /* NETSOURCELISTENER_H_ */
