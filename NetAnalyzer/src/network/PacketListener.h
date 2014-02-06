/*
 * PacketListener.h
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef PACKETLISTENER_H_
#define PACKETLISTENER_H_

struct Packet;

/**
 * interface to listen for incoming packets
 */
class PacketListener {

public:

	/** dtor */
	virtual ~PacketListener() {;}

	/** event is called on sniffed packets */
	virtual void onPacket(Packet* pkt) = 0;

};


#endif /* PACKETLISTENER_H_ */
