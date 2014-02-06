/*
 * PacketProcessor.h
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef PACKETPROCESSOR_H_
#define PACKETPROCESSOR_H_

struct Packet;

/**
 * interface for all packet processor.
 *
 * a processor may be responsible for a packet (e.g. IPv4, TCP, ..)
 * and can then modify this packet (e.g. re-assemble IPv4 fragments)
 *
 */
class PacketProcessor {

public:


	/** dtor */
	virtual ~PacketProcessor() {;}

	/** process the given packet. the processor MUST ignore the packet if he's not responsible for it */
	virtual void process(Packet* pkt) = 0;

};

#endif /* PACKETPROCESSOR_H_ */
