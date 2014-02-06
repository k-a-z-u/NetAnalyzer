/*
 * TCPReassembler.h
 *
 * provides re-assembly of TCP-streams
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef TCPREASSEMBLER_H_
#define TCPREASSEMBLER_H_

#include "TCPStream.h"
#include "IPv4Listener.h"

#include "../../misc/Average.h"

#include <map>
#include <vector>


class TCPConnectionListener;
struct Payload;

/**
 * store stats of the work of a TCP-reassembler
 * per direction here.
 */
struct TCPReassemblerDirStats {

	/** the number of packets received*/
	uint32_t numPackets = 0;

	/** the number of packets that only carry an ACK */
	uint32_t numAckOnly = 0;

	/** the number of packets without payload */
	uint32_t numWithoutPayload = 0;

	/** the number of packets with SYN-flag and payload */
	uint32_t numSynPayload = 0;

	/** the number of packets with wrong seq-number */
	uint32_t numSeqNumberMismatch = 0;

	/** the number of packets that had a wrong seq-number and could be re-ordered */
	uint32_t numSeqNumberReOrdered = 0;

	/** the number of packets probably sent more than once */
	uint32_t numRetransmits = 0;

	/** the number of errors */
	uint32_t numErrors = 0;

	/** total size of carried payload */
	uint64_t sizePayload = 0;

	/** track the average payload size of tcp packets (ignoring empty packets) */
	Average avgPayloadSize;

	/** track the average payload size of tcp packets (including empty packets) */
	Average avgPayloadEmptySize;

};


/**
 * store stats of the work of a TCP-reassembler
 */
struct TCPReassemblerStats {

	/** the number of id collissions within TCP-subsystem */
	uint32_t numIDCollision = 0;

	/** the number of tracked connections */
	uint32_t numConnections = 0;

	/** the number of currently open connections */
	uint32_t numOpenConnections = 0;

	/** the number of deleted connections (timeout) */
	uint32_t numPurgedConnections = 0;

	/** the number of ignored packets (wrong ID, ..) */
	uint32_t numPacketsIgnored = 0;


	/** global tcp-reassembler stats */
	TCPReassemblerDirStats global;

	/** traffic to the connection initiator */
	TCPReassemblerDirStats toInitiator;

	/** traffic from the connection initiator */
	TCPReassemblerDirStats fromInitiator;


};

/**
 * this class will re-assemble sniffed TCP streams
 * and provide a socket-like interface to them
 *
 */
class TCPReassembler : public IPv4Listener {
public:

	/** ctor */
	TCPReassembler();

	/** dtor */
	virtual ~TCPReassembler();

	void onIPv4Packet(PacketIPv4* pkt) override;

	/**
	 * set the listener to inform about accepted TCP-Connections.
	 * this listener MUST return another Listener which will then receive the stream's traffic
	 */
	void setConnectionListener(TCPConnectionListener* listener);

	/** get stats of this tcp-reassembler */
	const TCPReassemblerStats& getStats();

private:

	/** inform all listeners about this packet */
	void informListeners(PacketIPv4* pkt);

	/** close and cleanup the provided stream */
	void closeStream(TCPStream* stream);

	/** perform re-assembly on the given direction. returns false on errors */
	bool reAssemble(TCPStreamDirection& direction, uint32_t seqNumber, const Payload& payload, TCPStreamListener* listener);

	/** purge old connections (e.g. timeout) */
	void purgeOld();

	/** store TCP-Streams here */
	std::map<uint64_t, TCPStream*> streams;

	/** the listener to call for new connections */
	TCPConnectionListener* conListener;

	/** track the stats of this tcp reassembler */
	TCPReassemblerStats stats;

	/** delete connections after x ms of inactivity */
	uint32_t timeoutMs;

};

#endif /* TCPREASSEMBLER_H_ */
