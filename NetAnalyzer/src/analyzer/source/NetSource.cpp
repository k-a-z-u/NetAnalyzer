/*
 * NetSource.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#include "NetSource.h"
#include "NetSourceListener.h"

#include "../Summary.h"
#include "../Analyzer.h"
#include "../HTTPAnalyzer.h"

#include "../../network/Packet.h"
#include "../../network/PacketListener.h"
#include "../../network/PacketProvider.h"

#include "../../network/processors/IPv4Reassembler.h"
#include "../../network/processors/TCPReassembler.h"
#include "../../network/processors/TCPConnectionListener.h"

#include "../../http/HTTPDecoder.h"

#include "../../export/Exporter.h"



#define DBG_NET_SRC		"NetSrc"


NetSource::NetSource() : analyzer(nullptr), provider(nullptr) {

	debug(DBG_NET_SRC, DBG_LVL_INFO, "creating new NetSource");

	// create IPv4 and TCP processors
	ipv4_proc = new IPv4Reassembler();
	tcp_proc = new TCPReassembler();

	// attach them TCP re-assembler to the IPv4 re-assembler
	ipv4_proc->addListener(tcp_proc);


	// create a new PacketListener to provide global summary stats
	struct GlobalPacketListener : PacketListener {
			NetSource* parent;
			GlobalPacketListener(NetSource* parent) : parent(parent) {;}
			void onPacket(Packet* pkt) {

				// global statistics
				Summary& sum = parent->analyzer->getSummary();
				++sum.receivedPackets;
				sum.receivedBytes += pkt->getEthernet()->getTotalLength();
				sum.receivedProtocolOverheadBytes += pkt->getAllHeadersLength();

			}
		};

	packetListener = new GlobalPacketListener(this);



	// listen for TCP-connections to pass them to the HTTP handler
	struct TcpToHTTP : TCPConnectionListener {

		NetSource* parent;
		TcpToHTTP(NetSource* parent) : parent(parent) {;}
		TCPStreamListener* onTCPAccepted(uint16_t port) {

			// sanity check
			assertEQ(80, port, "received connection on port != 80!");
			assertNotNull(parent->analyzer, "the Analyzer of NetSource is a nullptr!");

			// debug
			debug(DBG_NET_SRC, DBG_LVL_INFO, "accepted on port:" << port);

			// create a new decoder that will decode the HTTP-stream
			// the TCP-System will destroy this HTTPDecoder when the TCP connection is closed
			HTTPDecoder* dec = new HTTPDecoder();

			// the decoder will send headers and payload to a new HTTPAnalyzer
			// this new HTTPAnalyzer will kill itself when the HTTP-transfer is close()d!!
			HTTPAnalyzer* httpAnalyzer = parent->analyzer->onHttpConnection();
			dec->addListener(*httpAnalyzer);

			// inform every listener about this newly created HTTPDecoder/HTTPAnalyzer
			// listeners may use this callback to register themselves within the decoder/analyzer
			for (auto l : parent->listener) {
				l->onNewTcpConnection( *httpAnalyzer, *dec );
			}

			// return the new decoder
			return dec;

		}
	};

	tcpListener = new TcpToHTTP(this);

	// attach the listener
	tcp_proc->setConnectionListener(tcpListener);

}

NetSource::~NetSource() {

	// debug
	debug(DBG_NET_SRC, DBG_LVL_INFO, "destroying analyzer");

	// cleanup
	delete ipv4_proc;
	delete tcp_proc;
	delete packetListener;
	delete tcpListener;

}

void NetSource::accept(Exporter& exp) {

	// we visit this exporter and it may then attach itself as our listener
	exp.visit(*this);

}

void NetSource::accept(Analyzer& analyzer) {

	Summary& summary = (Summary&) analyzer.getSummary();

	// register myself within the Summary
	summary.tcp.stats = &tcp_proc->getStats();
	summary.HTTP.stats = &HTTPDecoder::getStats();

	// set the analyzer this NetSource reports to
	this->analyzer = &analyzer;

}


void NetSource::setPacketProvider(PacketProvider* newProvider) {

	provider = newProvider;

	// attach listener for ipv4 / TCP re-assembly
	provider->addListener(ipv4_proc);

	// attach the listener for global packet facts
	provider->addListener(packetListener);

}

void NetSource::addListener(NetSourceListener* listener) {
	this->listener.push_back(listener);
}

void NetSource::start() {

	// start my provider
	if (provider != nullptr) {provider->start();}

}

void NetSource::stop() {

	// stop my provider
	if (provider != nullptr) {provider->stop();}

}
