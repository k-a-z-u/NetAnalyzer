/*
 * PacketProviderSniffer.h
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef PACKETPROVIDERSNIFFER_H_
#define PACKETPROVIDERSNIFFER_H_

#include <vector>
#include <string>

#include <pcap.h>

#include "../PacketProvider.h"

class PacketProcessor;
class PacketListener;

/**
 * the sniffer will directly interact with libpcap and triggers
 * the PacketListener interface whenever a packet has been captured.
 */
class PacketProviderSniffer : public PacketProvider {
public:

	/** singleton access */
	static PacketProviderSniffer& get();


	/** open device for sniffing */
	void open();

	/** start sniffing */
	void start();

	/** stop sniffing */
	void stop();

	/** close the sniffer */
	void close();


	/** set the device to use for sniffing */
	void setDevice(const std::string& device);

	/** set filter rule to apply */
	void setFilter(const std::string& rule);


	void addListener(PacketListener* listener) override;


	/** is the sniffer currently open? */
	bool isOpen();

	/** is the sniffer currently running? */
	bool isRunning();


private:

	/** hidden ctor (singleton) */
	PacketProviderSniffer();

	/** hidden dtor (singleton) */
	~PacketProviderSniffer();

	/** hidden copy ctor and assignment operator */
	PacketProviderSniffer(const PacketProviderSniffer&);
	PacketProviderSniffer& operator=(const PacketProviderSniffer&);


	/** callback to process one packet */
	static void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet);

	/** the device to use for sniffing */
	std::string device;

	/** listeners for incoming packets */
	std::vector<PacketListener*> listeners;

	/** the PCAP handle to work with */
	pcap_t* handle;

	/** is the sniffer open? */
	bool flagOpen;

	/** is the sniffer running? */
	bool flagRunning;

};

#endif /* PACKETPROVIDERSNIFFER_H_ */
