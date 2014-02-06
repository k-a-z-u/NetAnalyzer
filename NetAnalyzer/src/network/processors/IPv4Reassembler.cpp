/*
 * IPv4Reassembler.cpp
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#include "IPv4Reassembler.h"
#include "IPv4Listener.h"
#include "../Packet.h"
#include "../../Debug.h"
#include "../../Helper.h"

#include <cstring>

#define PURGE_AFTER_MS	5000
#define DBG_IPV4		"IPv4"


IPv4Reassembler::IPv4Reassembler() {

}

IPv4Reassembler::~IPv4Reassembler() {

	// clean-up all pending fragments
	for (auto it : storage) {delete it.second;}

}


void IPv4Reassembler::informListeners(PacketIPv4* pkt) {

	// send to next stage (if any)
	for (unsigned int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onIPv4Packet(pkt);
	}

}

void IPv4Reassembler::addListener(IPv4Listener* listener) {
	listeners.push_back(listener);
}


void IPv4Reassembler::purgeOld() {


	uint64_t ts = getTimeStampMS();

	// remove old entries from the cache
	std::map<uint16_t, IPv4Assembled*>::iterator it = storage.begin();
	while (it != storage.end()) {
		uint64_t age = ts - (*it).second->timeStamp;
		if (age > PURGE_AFTER_MS) {
			debug(DBG_IPV4, DBG_LVL_WARN, "purging one IPv4 entry");
			delete (*it).second;
			it = storage.erase(it);
		} else {
			++it;
		}
	}

}

void IPv4Reassembler::onPacket(Packet* pkt) {

	// silently ignore non IPv4 packets
	if (!pkt->getEthernet()->containsIPv4()) {return;}

	// get the IP-Paket we will be working on
	PacketIPv4* ipPkt = pkt->getEthernet()->getIPv4();

	// first and last fragment? -> inform listener
	if (ipPkt->getOffset() == 0 && !ipPkt->moreFragements()) {

		debug(DBG_IPV4, DBG_LVL_INFO, "non-fragmented packet -> informing next stage");

		// inform listeners
		informListeners(pkt->getEthernet()->getIPv4());

	} else {

		// packet needs re-assembly -> store
		debug(DBG_IPV4, DBG_LVL_INFO, "fragmented packet -> appending");

		// get parameters for assembly
		uint16_t id = ipPkt->getID();
		uint16_t offset = ipPkt->getOffset();

		IPv4Assembled* ass;

		// find already stored fragments or create a new entry
		if (storage.find(id) != storage.end()) {
			ass = storage[id];
		} else {
			ass = new IPv4Assembled;
			storage[id] = ass;
		}

		// touch this assembly (for purging it later)
		ass->timeStamp = getTimeStampMS();

		// append data to the buffer
		const Payload& payload = ipPkt->getPayload();
		memcpy((void*) &ass->buffer[offset], payload.data, payload.length);

		// set the number of received bytes
		ass->currentLength += payload.length;

		// is this packet has "moreFragments = false" -> we know the total size
		if (!ipPkt->moreFragements()) {
			ass->totalLength = offset + payload.length;
		}

		// assembly complete?
		if(ass->totalLength != 0 && ass->totalLength == ass->currentLength) {

			debug(DBG_IPV4, DBG_LVL_INFO, "fragment complete -> informing next stage");

			// create a temporal packet for next stage processors
			Payload ethHeader = pkt->getEthernet()->getHeader();
			Payload ipHeader = ipPkt->getHeader();

			// construct a temporal packet that contains the complete IP-Payload
			uint8_t tmp[ethHeader.length + ipHeader.length + ass->currentLength];

			// copy ethernet-header + ip-header + payload into the temporal buffer
			memcpy(tmp+0, ethHeader.data, ethHeader.length);
			memcpy(tmp+ethHeader.length, ipHeader.data, ipHeader.length);
			memcpy(tmp+ethHeader.length+ipHeader.length, ass->buffer, ass->currentLength);

			// create a temporal packet
			Packet* tmpPkt = Packet::wrap(tmp);
			tmpPkt->getEthernet()->getIPv4()->setTotalLength(ipPkt->getHeaderLength() + ass->currentLength);
			tmpPkt->getEthernet()->getIPv4()->setOffset(0);
			tmpPkt->getEthernet()->getIPv4()->setMoreFragments(false);
			//tmpPkt->dump();

			// inform listeners
			informListeners(tmpPkt->getEthernet()->getIPv4());

			// clean-up
			storage.erase(id);
			delete ass;

		}


	}

	// remove old entries
	purgeOld();

}
