/*
 * Packet.h
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "../Debug.h"
#include "../Helper.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* available IPv4 payload types */
#define IPv4_PAYLOAD_TCP		6
#define IPv4_PAYLOAD_UDP		17

/* available ethernet payload types */
#define ETHERNET_PAYLOAD_IPv4	0x0800


#define DBG_PACKET		"PACKET"


struct PacketIPv4;



/** describes a payload element */
struct Payload {

	/** the data of this payload */
	const uint8_t* data;

	/** the length of this payload */
	uint32_t length;

	/** ctor */
	public: Payload(const uint8_t* data, uint32_t length) : data(data), length(length) {;}

	/** hidden copy ctor */
	//private: Payload(const Payload& src) : data(0), length(0) {;}

};




#define TCP_FLAG_URG	(1 << 5)
#define TCP_FLAG_ACK	(1 << 4)
#define TCP_FLAG_PSH	(1 << 3)
#define TCP_FLAG_RST	(1 << 2)
#define TCP_FLAG_SYN	(1 << 1)
#define TCP_FLAG_FIN	(1 << 0)

/** structural representation of a TCP packet */
struct PacketTCP {

	uint8_t srcPort[2];
	uint8_t dstPort[2];
	uint8_t seqNr[4];
	uint32_t ackNr;
	uint8_t offsetAndFlags[2];


	/* get the tcp sequence number */
	uint32_t getSeqNumber() {return seqNr[0] << 24 | seqNr[1] << 16 | seqNr[2] << 8 | seqNr[3] << 0;}

	/* set the tcp sequence number */
	void setSeqNumber(uint32_t seq) {
		seqNr[0] = seq >> 24;
		seqNr[1] = seq >> 16;
		seqNr[2] = seq >> 8;
		seqNr[3] = seq >> 0;
	}



	/** is the URG flag set? */
	bool isUrgent()	{return offsetAndFlags[1] & TCP_FLAG_URG;}

	/** is the ACK flag set? */
	bool isAck()	{return offsetAndFlags[1] & TCP_FLAG_ACK;}

	/** is the PSH flag set? */
	bool isPush()	{return offsetAndFlags[1] & TCP_FLAG_PSH;}

	/** is the RST flag set? */
	bool isReset()	{return offsetAndFlags[1] & TCP_FLAG_RST;}

	/** is the SYN flag set? */
	bool isSyn()	{return offsetAndFlags[1] & TCP_FLAG_SYN;}

	/** is the FIN flag set? */
	bool isFin()	{return offsetAndFlags[1] & TCP_FLAG_FIN;}


	/** get all flags as 6-bit value. (use with TCP_FLAG_xxx) */
	uint8_t getFlags() {return offsetAndFlags[1] & 0b00111111;}

	/** set the flags for this TCP packet */
	void setFlags(bool urg, bool ack, bool psh, bool rst, bool syn, bool fin) {
		offsetAndFlags[1] &= ~0b00111111;
		offsetAndFlags[1] |= (urg) ? (1 << 5) : (0);
		offsetAndFlags[1] |= (ack) ? (1 << 4) : (0);
		offsetAndFlags[1] |= (psh) ? (1 << 3) : (0);
		offsetAndFlags[1] |= (rst) ? (1 << 2) : (0);
		offsetAndFlags[1] |= (syn) ? (1 << 1) : (0);
		offsetAndFlags[1] |= (fin) ? (1 << 0) : (0);
	}


	/** get the source port */
	uint16_t getSrcPort() {return srcPort[0] << 8 | srcPort[1];}

	/** get the destination port */
	uint16_t getDstPort() {return dstPort[0] << 8 | dstPort[1];}

	/** set the source port */
	void setSrcPort(uint16_t port) {srcPort[0] = port >> 8; srcPort[1] = port & 0xFF;}

	/** set the destination port */
	void setDstPort(uint16_t port) {dstPort[0] = port >> 8; dstPort[1] = port & 0xFF;}



	/** get the length of the TCP header */
	uint8_t getHeaderLength() {return (offsetAndFlags[0] >> 4) << 2;}

	/** set the length of the TCP header */
	void setHeaderLength(uint8_t length) {
		assertTrue(length >= 20, "TCP's header length must be at least 20!");
		assertTrue((length % 4) == 0, "TCP's header length must be a multiple of 4!");
		offsetAndFlags[0] &= ~0b11110000;
		offsetAndFlags[0] |= ((length >> 2) << 4);
	}


	/** get the payload of this TCP packet */
	Payload getPayload(uint16_t ipv4PayloadLength) {return Payload( (uint8_t*) this + getHeaderLength(), ipv4PayloadLength - getHeaderLength());}


	/** dump info of this packet */
	void dump() {
		std::cout << "\t\tTCP" << std::endl;
		std::cout << "\t\t- src port: " << getSrcPort() << std::endl;
		std::cout << "\t\t- dst port: " << getDstPort() << std::endl;
		std::cout << "\t\t- header-len: " << (int) getHeaderLength() << std::endl;
		std::cout << "\t\t- FLAGS: ";
		if (isUrgent()) {std::cout << "URG ";}
		if (isAck())	{std::cout << "ACK ";}
		if (isPush())	{std::cout << "PSH ";}
		if (isReset())	{std::cout << "RST ";}
		if (isSyn())	{std::cout << "SYN ";}
		if (isFin())	{std::cout << "FIN ";}
		std::cout << std::endl;

	}

};

/** structural representation of an IPv4 packet */
struct PacketIPv4 {

	uint8_t versionAndHeaderLength;
	uint8_t typeOfService;
	uint8_t length[2];
	uint8_t id[2];
	uint8_t flagsAndOffset[2];
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checkSum;
	uint8_t srcIP[4];
	uint8_t dstIP[4];

	/** get the IPv4 ID of this packet */
	uint16_t getID() {return id[0] << 8 | id[1];}

	/** get the length of the IPv4 header (in bytes) */
	uint8_t getHeaderLength() {return (versionAndHeaderLength & 0b00001111) << 2;}

	/** set the length of the IPv4 header (in bytes) */
	void setHeaderLength(uint8_t length) {
		assertTrue(length >= 20, "IPv4's header length must be at least 20!");
		assertTrue((length % 4) == 0, "IPv4's header length must be a multiple of 4!");
		versionAndHeaderLength &= ~0b00001111;
		versionAndHeaderLength |= (length >> 2) & 0b00001111;
	}

	/** get the source-IP as uint32 */
	uint32_t getSrcIP() {return srcIP[0] << 24 | srcIP[1] << 16 | srcIP[2] << 8 | srcIP[3];}

	/** get the source-IP as uint32 */
	uint32_t getDstIP() {return dstIP[0] << 24 | dstIP[1] << 16 | dstIP[2] << 8 | dstIP[3];}


	/** get the length of this IPv4 packet INCLUDING the header */
	uint16_t getTotalLength() {return length[0] << 8 | length[1];}

	/** set the total length of this IPv4 packet INCLUDING the header */
	void setTotalLength(uint16_t totalLength) {length[0] = totalLength >> 8; length[1] = totalLength & 0xFF;}


	/** get the length of all headers until real payload (IP + TCP, ..) */
	uint32_t getAllHeadersLength() {
		uint32_t length = getHeaderLength();
		if		(containsTCP())	{length += getTCP()->getHeaderLength();}
		else if	(containsUDP())	{length += 0;} // TODO
		return length;
	}


	/** are more fragments of this packet on their way? */
	bool moreFragements() {return (flagsAndOffset[0] & (1<<5));}

	/** set whether there will be more fragments for this IPv4 packet */
	void setMoreFragments(bool more) {flagsAndOffset[0] &= ~(1<<5); flagsAndOffset[0] |= (more) ? (1<<5) : (0);}


	/** get the offset of this IPv4 packet (if fragmented) */
	uint16_t getOffset() {return ((flagsAndOffset[0] & 0b00011111) << 8 | flagsAndOffset[1]) * 8;}

	/** set the offset of this IPv4 packet (if fragmented) */
	void setOffset(uint16_t offset) {
		if (offset % 8 != 0) {error("Packet", "IPv4 offset must be a multiple of 8!");}
		offset /= 8;
		flagsAndOffset[0] = flagsAndOffset[0] & 0b11100000;
		flagsAndOffset[1] = 0;
		flagsAndOffset[0] |= (offset >> 8) & 0b00011111;
		flagsAndOffset[1]  = (offset >> 0) & 0b11111111;
	}


	/** get the length of the payload */
	uint32_t getPayloadLength() {return getTotalLength() - getHeaderLength();}


	/** get the header of this IPv4 packet */
	Payload getHeader() {return Payload( (uint8_t*) this, getHeaderLength() );}

	/** get the payload of this packet */
	Payload getPayload() {return Payload( (uint8_t*) this + getHeaderLength(), getPayloadLength());}


	/** get the TCP payload */
	struct PacketTCP* getTCP() {return (struct PacketTCP*) ((uint8_t*)this + getHeaderLength());}


	/** does this packet contain TCP payload? */
	bool containsTCP() {return protocol == IPv4_PAYLOAD_TCP;}

	/** does this packet contain TCP payload? */
	bool containsUDP() {return protocol == IPv4_PAYLOAD_UDP;}


	/** set the type of the inner protocol. e.g. "IPv4_PAYLOAD_TCP" */
	void setProtocol(uint8_t IPv4_PAYLOAD) {protocol = IPv4_PAYLOAD;}


	/** dump info of this packet */
	void dump() {
		std::cout << "\tIPv4" << std::endl;
		std::cout << "\t- src-IP: " << (int)srcIP[0] << "." << (int)srcIP[1] << "." << (int)srcIP[2] << "." << (int)srcIP[3] << std::endl;
		std::cout << "\t- dst-IP: " << (int)dstIP[0] << "." << (int)dstIP[1] << "." << (int)dstIP[2] << "." << (int)dstIP[3] << std::endl;
		std::cout << "\t- header size: " << (int)getHeaderLength() << std::endl;
		std::cout << "\t- payload size: " << getPayloadLength() << std::endl;
		std::cout << "\t- offset: " << getOffset() << std::endl;
		if (containsTCP()) {getTCP()->dump();}
	}

};


/** represents an ethernet packet with payload */
struct PacketEthernet {

	uint8_t srcMAC[6];
	uint8_t dstMAC[6];
	uint8_t type[2];

	/** set the type of payload this ethernet packet contains */
	void setPayloadType(uint16_t newType) {type[0] = newType >> 8; type[1] = newType & 0xFF;}

	/** get the type of payload this ethernet packet contains */
	uint16_t getPayloadType() {return type[0] << 8 | type[1];}

	/** does this packet contain IPv4 payload? */
	bool containsIPv4() {return getPayloadType() == ETHERNET_PAYLOAD_IPv4;}

	/** get the length of the ethernet header */
	uint8_t getHeaderLength() {return 14;}


	/** get the IPv4 payload */
	struct PacketIPv4* getIPv4() {
		return (struct PacketIPv4*) ((uint8_t*)this + getHeaderLength());
	}

	/** get the header of this Ethernet packet */
	Payload getHeader() {return Payload( (uint8_t*) this, getHeaderLength() );}


	/** get the total length of this packet including payload */
	uint32_t getTotalLength() {
		uint32_t length = getHeaderLength();
		if (containsIPv4()) {length += getIPv4()->getTotalLength();}
		return length;
	}

	/** get the length of all headers until real payload (ethernet + IP + tcp, ..) */
	uint32_t getAllHeadersLength() {
		uint32_t length = getHeaderLength();
		if (containsIPv4()) {length += getIPv4()->getAllHeadersLength();}
		return length;
	}

	/** dump info of this packet */
	void dump() {
		std::cout << "ethernet" << std::endl;
		std::cout << "- src-mac: ";
		printHex(srcMAC, 6);
		std::cout << "- dst-mac: ";
		printHex(dstMAC, 6);
		if (containsIPv4()) {getIPv4()->dump();}
	}

};

/** structural access to sniffed lib-pcap data */
struct Packet {

	/** get the ethernet payload */
	struct PacketEthernet* getEthernet() {
		return (struct PacketEthernet*) this;
	}

	/** get the total length of this packet including payload */
	uint32_t getTotalLength() {return getEthernet()->getTotalLength();}

	/** get the length of all headers until real payload (ethernet + IP + tcp, ..) */
	uint32_t getAllHeadersLength() {return getEthernet()->getAllHeadersLength();}

private:

	/** hidden ctor. use wrap() instead! */
	Packet() {;}


public:

	/** wrap a Packet around the sniffed result of lib-pcap */
	static Packet* wrap(const u_char* packet) {

		// simply wrap the Packet-struct around the sniffed data-array
		Packet* pkt = (Packet*) &packet[0];
		return pkt;

	}

	/** wrap a Packet around this data array */
	static Packet* wrap(uint8_t* data) {
		return (Packet*) data;
	}

	/** dump packet details */
	void dump() {
		getEthernet()->dump();
		std::cout << std::endl;
	}

	/** is this an IPv4 packet? */
	bool isIPv4() {return getEthernet()->getPayloadType() == ETHERNET_PAYLOAD_IPv4;}

	/** is this a TCP packet? */
	bool isTCP() {
		if (!isIPv4()) {return false;}
		return (getEthernet()->getIPv4()->protocol == 6);
	}

	/** is this a UDP packet? */
	bool isUDP() {
		if (!isIPv4()) {return false;}
		return (getEthernet()->getIPv4()->protocol == 17);
	}

	/** get type as string */
	std::string getTypeString() {
		switch (getEthernet()->getPayloadType()) {
		case 0x0008:
			switch(getEthernet()->getIPv4()->protocol) {
			case 6:		return "IPv4 - TCP";
			case 17:	return "IPv4 - UDP";
			}
			return "IPv4 - unknown";
			default:		return "unknown";
		}
	}




};




#endif /* PACKET_H_ */
