/*
 * PacketProviderSniffer.cpp
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#include "PacketProviderSniffer.h"
#include "../Packet.h"
#include "../PacketListener.h"
#include "../../Debug.h"

#include <cstring>
#include <cstdlib>

#define RING_BUFFER_SIZE		(32*1024*1024)
#define CAPTURE_BUFFER_SIZE 	4096
#define USE_PROMISCIOUS_MODE	0
#define DBG_SNIFFER				"SNIFFER"

// Workaround for older pcap installations. PCAP_NETMASK_UNKNOWN was introduced in 1.3.
// See http://fossies.org/dox/libpcap-1.3.0/pcap_2pcap_8h.html
#ifdef PCAP_NETMASK_UNKNOWN
#define PCAP_NETMASK_UNKNOWN	0xffffffff
#endif




PacketProviderSniffer::PacketProviderSniffer() : handle(nullptr), flagOpen(false), flagRunning(false) {
	;
}

PacketProviderSniffer::~PacketProviderSniffer() {

	// cleanup
	if (isRunning())	{stop();}
	if (isOpen())		{close();}

}

void PacketProviderSniffer::setDevice(const std::string& device) {this->device = device;}

bool PacketProviderSniffer::isOpen() {return flagOpen;}

bool PacketProviderSniffer::isRunning() {return flagRunning;}

void PacketProviderSniffer::open() {

	// sanity check
	if (flagOpen) {error(DBG_SNIFFER, "can't open() twice!");}

	char errbuf[PCAP_ERRBUF_SIZE] = {0};
	//memset(errbuf,0,PCAP_ERRBUF_SIZE);

	// device configured or use default device?
	if (device.empty()) {
		debug(DBG_SNIFFER, DBG_LVL_WARN, "no device set, using default device!");
		const char* tmpDev;
		if ( (tmpDev = pcap_lookupdev(errbuf)) == nullptr) {error(DBG_SNIFFER, errbuf);}
		device = std::string(tmpDev);
	}

	debug(DBG_SNIFFER, DBG_LVL_INFO, "opening device " << device);


	// open device
	//if ( (handle = pcap_open_live(device.c_str(), CAPTURE_BUFFER_SIZE, USE_PROMISCIOUS_MODE,  512, errbuf)) == nullptr) {
	//	error(DBG_SNIFFER, errbuf);
	//}
	// open the network device
	if ( (handle = pcap_create(device.c_str(), errbuf)) == nullptr) {
		error(DBG_SNIFFER, errbuf);
	}

	// configure some options
	pcap_set_snaplen(handle, CAPTURE_BUFFER_SIZE);
	pcap_set_buffer_size(handle, RING_BUFFER_SIZE);
	pcap_set_promisc(handle, USE_PROMISCIOUS_MODE);
	pcap_set_timeout(handle, 500);

	// activate capturing (when using pcap_create())
	int ret = pcap_activate(handle);
	if (ret != 0) {
		error(DBG_SNIFFER, "error while opening device " << device << "! '" << pcap_geterr(handle) << "' (code:" << ret << ")");
	}

	debug(DBG_SNIFFER, DBG_LVL_INFO, "device is now open");

	// mark as open
	flagOpen = true;

}

void PacketProviderSniffer::close() {

	// sanity check
	if (!flagOpen) {error(DBG_SNIFFER, "can't close(). already closed!");}

	pcap_close(handle);

	// mark as closed
	flagOpen = false;

}

void PacketProviderSniffer::start() {

	// sanity check
	if (!flagOpen)		{error(DBG_SNIFFER, "open() the Sniffer before starting!");}
	if (flagRunning)	{error(DBG_SNIFFER, "can't start() the Sniffer twice!");}

	debug(DBG_SNIFFER, DBG_LVL_INFO, "starting...");
	int count = 0;

	// mark as running here, as the following code will block until done
	flagRunning = true;

	// loop forever and call Sniffer::processPacket() for every packet
	while(flagRunning) {
		if (pcap_dispatch(handle, 0, PacketProviderSniffer::processPacket, (u_char *) &count) == -1) {
			error(DBG_SNIFFER, pcap_geterr(handle));
		}
	}

}

void PacketProviderSniffer::stop() {

	// sanity checks
	if (!flagRunning) {error(DBG_SNIFFER, "start() the Sniffer before using stop()!");}

	// mark as stopped
	flagRunning = false;

	// stop the sniffing-loop
	pcap_breakloop(handle);

	// update stats
	struct pcap_stat stats;
	pcap_stats(handle, &stats);
	debug(DBG_SNIFFER, DBG_LVL_INFO, "dropped packets: " << (stats.ps_ifdrop + stats.ps_drop));



	debug(DBG_SNIFFER, DBG_LVL_INFO, "sniffer stopped");

}

void PacketProviderSniffer::setFilter(const std::string& rule) {

	// sanity check
	if (handle == nullptr) {error(DBG_SNIFFER, "open() the Sniffer before adding filters!");}

	// compile berkeley-packet-filter
	struct bpf_program pgm;
	int ret = pcap_compile(handle, &pgm, rule.c_str(), 1, PCAP_NETMASK_UNKNOWN);
	if (ret < 0) {error(DBG_SNIFFER, "error while compiling filter '" << rule << "': " << pcap_geterr(handle));}
	debug(DBG_SNIFFER, DBG_LVL_INFO, "filter compiled");

	// apply compiled filter
	ret = pcap_setfilter(handle, &pgm);
	if (ret < 0) {error(DBG_SNIFFER, "error while activating filter '" << rule << "': " << pcap_geterr(handle));}
	debug(DBG_SNIFFER, DBG_LVL_INFO, "filter activated");

	// cleanup
	pcap_freecode(&pgm);

}

void PacketProviderSniffer::addListener(PacketListener* listener) {
	listeners.push_back(listener);
}


void PacketProviderSniffer::processPacket(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {

	(void) arg;
	(void) pkthdr;

	// get the Sniffer-Singleton
	PacketProviderSniffer& sniffer = PacketProviderSniffer::get();

	// create packet
	Packet* pkt = Packet::wrap(packet);
	//pkt->dump();
	//debug("------------------------------------------------");

	// inform listeners
	for (auto& listener : sniffer.listeners) {
		listener->onPacket(pkt);
	}

	// NOTE: lib-pcap will free() the packet, after this method returns!

}

PacketProviderSniffer& PacketProviderSniffer::get() {
	static PacketProviderSniffer sniffer; // classic Meyers Singleton
	return sniffer;
}
