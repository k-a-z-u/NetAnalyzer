/*
 * TestIPv4.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: kazu
 */



#ifdef WITH_TESTS

#include "Test.h"
#include "../network/Packet.h"
#include "../network/processors/IPv4Reassembler.h"
#include "../network/processors/IPv4Listener.h"
#include "../buffer/DataBuffer.h"

/** calling this method should not lead to any memory leaks */
TEST(IPv4, LeakCheck) {

	IPv4Reassembler ipv4;

	// wrap packet around buffer
	uint8_t data[2048] = {0};
	Packet* pkt = (Packet*) &data;
	PacketIPv4* pktIpv4 = pkt->getEthernet()->getIPv4();

	pkt->getEthernet()->setPayloadType(ETHERNET_PAYLOAD_IPv4);

	// send 10 bytes of payload at a give offset
	pktIpv4->setHeaderLength(20);
	pktIpv4->setTotalLength(30);
	pktIpv4->setOffset(8);
	pktIpv4->setMoreFragments(false);

	// process
	ipv4.onPacket(pkt);


}

/** perform reassembly check */
TEST(IPv4, Reassembly) {

	IPv4Reassembler ipv4;
	static DataBuffer db;

	class : public IPv4Listener {
		virtual void onIPv4Packet(PacketIPv4* pkt) {
			Payload p = pkt->getPayload();
			db.append(p.data, p.length);
		}
	} myListener;

	ipv4.addListener(&myListener);

	// wrap packet around buffer
	uint8_t data[2048] = {0};
	Packet* pkt = (Packet*) &data;
	PacketIPv4* pktIpv4 = pkt->getEthernet()->getIPv4();

	pkt->getEthernet()->setPayloadType(ETHERNET_PAYLOAD_IPv4);

	// send 8 bytes of payload at a given offset
	memcpy(&data[14+20], "12345678", 8);
	pktIpv4->setHeaderLength(20);
	pktIpv4->setTotalLength(28);
	pktIpv4->setOffset(8);
	pktIpv4->setMoreFragments(true);
	ipv4.onPacket(pkt);

	// send 8 bytes of payload at a given offset
	memcpy(&data[14+20], "98765432", 8);
	pktIpv4->setHeaderLength(20);
	pktIpv4->setTotalLength(28);
	pktIpv4->setOffset(16);
	pktIpv4->setMoreFragments(false);
	ipv4.onPacket(pkt);

	// send 8 bytes of payload at offset 0
	memcpy(&data[14+20], "ABCDEFGH", 8);
	pktIpv4->setHeaderLength(20);
	pktIpv4->setTotalLength(28);
	pktIpv4->setOffset(0);
	pktIpv4->setMoreFragments(true);
	ipv4.onPacket(pkt);

	// check buffer content
	DataBufferData ret = db.get();
	ASSERT_DATA_EQ("ABCDEFGH1234567898765432", ret.data, 24);

}

/** perform reassembly check */
TEST(IPv4, DirectPacket) {

	IPv4Reassembler ipv4;
	static DataBuffer db;

	class : public IPv4Listener {
		virtual void onIPv4Packet(PacketIPv4* pkt) {
			Payload p = pkt->getPayload();
			db.append(p.data, p.length);
		}
	} myListener;

	ipv4.addListener(&myListener);

	// wrap packet around buffer
	uint8_t data[2048] = {0};
	Packet* pkt = (Packet*) &data;
	PacketIPv4* pktIpv4 = pkt->getEthernet()->getIPv4();

	pkt->getEthernet()->setPayloadType(ETHERNET_PAYLOAD_IPv4);

	// send 8 bytes of payload at offset 0
	memcpy(&data[14+20], "aabbccdd", 8);
	pktIpv4->setHeaderLength(20);
	pktIpv4->setTotalLength(28);
	pktIpv4->setOffset(0);
	pktIpv4->setMoreFragments(false);
	ipv4.onPacket(pkt);

	// check buffer content
	DataBufferData ret = db.get();
	ASSERT_DATA_EQ("aabbccdd", ret.data, 8);

}

#endif
