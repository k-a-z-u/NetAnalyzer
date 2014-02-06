/*
 * IPv4Listener.h
 *
 * listen for re-assembled IPv4 Packets
 *
 *  Created on: Apr 8, 2013
 *      Author: kazu
 */

#ifndef IPV4LISTENER_H_
#define IPV4LISTENER_H_

struct PacketIPv4;

/**
 * listen for re-assembled IPv4 Packets
 */
class IPv4Listener {

public:

	virtual ~IPv4Listener() {;}

	/** called when a re-assmbled IPv4 Packet has been read */
	virtual void onIPv4Packet(PacketIPv4* pkt) = 0;

};

#endif /* IPV4LISTENER_H_ */
