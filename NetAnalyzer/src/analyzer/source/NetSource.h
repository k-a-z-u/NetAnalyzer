/*
 * NetSource.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef NETSOURCE_H_
#define NETSOURCE_H_

#include "../Summary.h"


#include <string>
#include <vector>

#include "../HTTPDataSource.h"

// forward declarations
class PacketProvider;
class NetSourceListener;

class IPv4Reassembler;
class TCPReassembler;
class TCPConnectionListener;
class PacketListener;

/**
 * the NetSource will use a PacketProvider (e.g. Sniffer) to  listen for TCP-Connections on
 * several ports (currently only HTTP) and will then inform the analyzer
 * about this new stream
 */
class NetSource : public HTTPDataSource {

public:

	/** ctor */
	NetSource();

	/** dtor */
	virtual ~NetSource();

	/** set the module that provides packets (e.g. Sniffer or PCAP-File) */
	void setPacketProvider(PacketProvider* provider);

	/**
	 * add a new listener to inform about internal events
	 * e.g. for Exporters to register themselves for additional information
	 */
	void addListener(NetSourceListener* listener);

	void accept(Analyzer& analyzer) override;

	void accept(Exporter& exp) override;

	void start() override;

	void stop() override;

private:

	/** hidden copy ctor and assignment operator */
	NetSource(const NetSource&);
	NetSource& operator=(const NetSource&);


	/** the analyzer we will inform about new HttpConnections */
	Analyzer* analyzer;

	/** the module (sniffer, pcap-file) that triggers an event whenever a packet is received */
	PacketProvider* provider;

	/** re-assmebly of IPv4 packets */
	IPv4Reassembler* ipv4_proc;

	/** following TCP-streams */
	TCPReassembler* tcp_proc;

	/** the global packet listener for packet-based-facts */
	PacketListener* packetListener;

	/** listen for TCP connections to pass them to the HTTP analyzer */
	TCPConnectionListener* tcpListener;

	/** all NetAnalyzer listeners */
	std::vector<NetSourceListener*> listener;

};

#endif /* NETANALYZER_H_ */
