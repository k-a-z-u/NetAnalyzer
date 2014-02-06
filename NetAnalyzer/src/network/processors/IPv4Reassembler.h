/*
 * IPv4Reassembler.h
 *
 * provides re-assembly of IPv4 packets
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef IPV4REASSEMBLER_H_
#define IPV4REASSEMBLER_H_

#include "../PacketListener.h"

#include <map>
#include <vector>
#include <cstdint>

class IPv4Listener;
struct PacketIPv4;
struct Payload;

/**
 * store re-assmbled packets here
 *
 * when the packet with the "more-fragments = false" flag is received
 * we know the total size of the buffer. thus it's then possible
 * to check whether all fragments have been received
 */
struct IPv4Assembled {

	/** the buffer where to combine all fragments */
	uint8_t buffer[64*1024];

	/** the number of already received bytes */
	uint32_t currentLength;

	/** this value will be set when the last fragment is received */
	uint32_t totalLength;

	// timestamp to track the age of this assembly. old entries will be purged
	uint64_t timeStamp;

	/** ctor */
	public: IPv4Assembled() : currentLength(0), totalLength(0), timeStamp(0) {;}

};

/**
 * re-assembles fragmented IPv4 packets and provides them to it's listeners
 */
class IPv4Reassembler : public PacketListener {

public:

	/** ctor */
	IPv4Reassembler();

	/** dtor */
	virtual ~IPv4Reassembler();

	void onPacket(Packet* pkt) override;

	/** add a new listener to inform about incoming IPv4 packets */
	void addListener(IPv4Listener* listener);

private:

	/** purge all old entries from the buffer */
	void purgeOld();

	/** inform all listeners about this packet */
	void informListeners(PacketIPv4* pkt);

	/** store assembled packets here */
	std::map<uint16_t, IPv4Assembled*> storage;

	/** all listeners to inform */
	std::vector<IPv4Listener*> listeners;

};

#endif /* IPV4REASSEMBLER_H_ */
