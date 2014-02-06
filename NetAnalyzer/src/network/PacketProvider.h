/*
 * PacketProvider.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#ifndef PACKETPROVIDER_H_
#define PACKETPROVIDER_H_

// forward declarations
class PacketListener;

/**
 * a packet provider will trigger the PacketListener interface
 * whenever a packet has been captured/sniffer/received/...
 */
class PacketProvider {

public:

	/** dtor */
	virtual ~PacketProvider() {;}

	/** add a PacketListener that will receive any captured packet */
	virtual void addListener(PacketListener* listener) = 0;

	/** start the packet provider. (start triggering PacketListener interface) */
	virtual void start() = 0;

	/** stop the packet provider. (stop triggering PacketProvider interface) */
	virtual void stop() = 0;

};


#endif /* PACKETPROVIDER_H_ */
