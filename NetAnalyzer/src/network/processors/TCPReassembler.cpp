/*
 * TCPReassembler.cpp
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#include "TCPReassembler.h"
#include "../Packet.h"
#include "../../Debug.h"

#include "TCPStreamListener.h"
#include "TCPConnectionListener.h"

#define DBG_TCP			"TCP"

// remove connections with inactivity of 5 minutes
#define TCP_DEF_TIMEOUT	1000*60*5
//#define TCP_DEF_TIMEOUT 1000



TCPReassembler::TCPReassembler() : conListener(nullptr), timeoutMs(TCP_DEF_TIMEOUT) {
	;
}

TCPReassembler::~TCPReassembler() {

	debug(DBG_TCP, DBG_LVL_DEBUG, "deleting TCP-Reassembler");

	// remove all still active streams
	// this SHOULD not be necessary!
	if (streams.size() != 0) {
		debug(DBG_TCP, DBG_LVL_WARN, "had to close " << streams.size() << " TCP-streams on shutdown. leaks??");
	}
	for (auto it : streams) {
		it.second->listener->onClose();
		delete it.second->listener;
		delete it.second;
	}

}


void TCPReassembler::setConnectionListener(TCPConnectionListener* listener) {
	conListener = listener;
}

const TCPReassemblerStats& TCPReassembler::getStats() {
	return stats;
}

bool TCPReassembler::reAssemble(TCPStreamDirection& dir, uint32_t seqNumber, const Payload& payload, TCPStreamListener* listener) {

	// stats-pointer per direction
	TCPReassemblerDirStats* dirStats = (dir.isFromInitiator) ? (&stats.fromInitiator) : (&stats.toInitiator);

	// check if seq-number matches
	if (dir.nextSeqNumber == seqNumber) {

		debug(DBG_TCP, DBG_LVL_INFO, "seq-number OK! -> appending " << payload.length << " bytes");

		// adjust seq-number
		dir.nextSeqNumber += payload.length;

		// inform listener
		if (dir.isFromInitiator) {
			listener->onDataFromInitiator(payload);
		} else {
			listener->onDataToInitiator(payload);
		}

		// now check if we have buffered some previously buffered packets that now
		// match the new sequence number
		TCPBufferedSegment* seg = nullptr;
		while( (seg = dir.buffer.get(dir.nextSeqNumber)) != nullptr) {

			++stats.global.numSeqNumberReOrdered;
			++dirStats->numSeqNumberReOrdered;

			debug(DBG_TCP, DBG_LVL_DEBUG, "found previously buffered element!");

			// create payload-wrapper
			Payload payloadSeg(seg->data, seg->length);

			// inform correct listener
			if (dir.isFromInitiator) {
				listener->onDataFromInitiator(payloadSeg);
			} else {
				listener->onDataToInitiator(payloadSeg);
			}

			// release memory
			delete seg;
			seg = nullptr;

		}

	} else {

		// ignore retransmits
		if (dir.nextSeqNumber > seqNumber) {
			debug(DBG_TCP, DBG_LVL_WARN, "found retransmit -> skipping");
			++stats.global.numRetransmits;
			++dirStats->numRetransmits;
			return true;
		}

		// check whether buffering is possible
		if (dir.buffer.isFull()) {
			debug(DBG_TCP, DBG_LVL_ERR, "seq-buffer is full -> closing connection!");
			++stats.global.numErrors;
			++dirStats->numErrors;
			return false;
		}

		debug(DBG_TCP, DBG_LVL_DEBUG, "seq-number mismatch! (" << dir.nextSeqNumber << ":" << seqNumber << ") -> buffering " << payload.length << " bytes at offset " << (seqNumber - dir.nextSeqNumber));

		// append to buffer
		dir.buffer.append(seqNumber, payload.data, payload.length);

		++stats.global.numSeqNumberMismatch;
		++dirStats->numSeqNumberMismatch;

	}

	// everything fine
	return true;

}

void TCPReassembler::onIPv4Packet(PacketIPv4* pkt) {

	// sanity checks
	assertNotNull(conListener, "set a connection-listener first before using the TCP-Reassembler!");

	// silently ignore non tcp packets
	if (!pkt->containsTCP()) {
		error(DBG_TCP, "got non-TCP packet!!!");
		return;
	}

	++stats.global.numPackets;

	// get the TCP-Packet
	PacketTCP* tcp = pkt->getTCP();

	// the ID will be created using both ports and IPs
	// the ID will thus be the same for BOTH directions!!
	// we store the local-port to distinguish between incoming and outgoing later
	const uint64_t ipSUM = (uint64_t)pkt->getDstIP() + (uint64_t)pkt->getSrcIP();
	const uint64_t portSUM = (uint64_t)pkt->getTCP()->getSrcPort() + (uint64_t)pkt->getTCP()->getDstPort();
	const uint64_t id = (ipSUM << 24) + portSUM;


	// the TCP-Stream (directional) all following functions will use
	TCPStream* stream = nullptr;


	// if this is a SYN packet (without ACK)
	// we have found a new connection-request
	if (tcp->isSyn() && !tcp->isAck()) {

		// sanity check.
		// there MUST NOT be a stream with this ID!
		// FIXME better solution to avoid collisions?
		if (streams.find(id) != streams.end()) {

			++stats.numIDCollision;
			debug(DBG_TCP, DBG_LVL_ERR, "new connection but ID already in use -> skipping");

			// we also need to close the connection with the existing ID
			// otherwise the traffic of both connection would be miked and lead to further problems!
			closeStream(streams[id]);

			return;

		}
		//assertTrue( streams.find(id) == streams.end(), "TCP id collision detected!" );

		++stats.numConnections;
		++stats.numOpenConnections;
		++stats.fromInitiator.numPackets;

		debug(DBG_TCP, DBG_LVL_INFO, "new connection with ID " << id << " (open connections: " << stats.numOpenConnections << ")");

		// create a new stream for this connection
		stream = new TCPStream();
		stream->id = id;

		// store the stream
		streams[id] = stream;

		// store the local port (= dst-port) the connection was accepted on,
		// to distinguish between incoming and outgoing later
		stream->localPort = tcp->getDstPort();

		// set first sequence number
		stream->fromInitiator.nextSeqNumber = tcp->getSeqNumber();

		// call the connection listener.
		// this listener MUST return a TCPStreamListener which will receive the data-flow
		stream->listener = conListener->onTCPAccepted(tcp->getDstPort());

		// debug assertions
		assertNotNull(stream->listener, "the listener of the TCP-Stream has not been set. Is the TcpConnectionListener working correctly?");

		// inform the stream-listener
		stream->listener->onOpen();

	} else if (tcp->isSyn() && tcp->isAck()) {

		++stats.toInitiator.numPackets;

		// proceed with existing streams only
		if (streams.find(id) == streams.end()) {
			++stats.numPacketsIgnored;
			debug(DBG_TCP, DBG_LVL_WARN, "unknown connection ID (in SYN-ACK) -> skipping");
			return;
		}

		// get stream by it's id
		stream = streams[id];

		// set fist sequence number
		stream->toInitiator.nextSeqNumber = tcp->getSeqNumber();

	} else {

		// proceed with existing streams only
		if (streams.find(id) == streams.end()) {
			++stats.numPacketsIgnored;
			debug(DBG_TCP, DBG_LVL_WARN, "unknown connection ID -> skipping");
			return;
		}

		// get the stream by it's id
		stream = streams[id];

	}

	// we should only be here if everything is fine

	// update timestamp for timeout detection
	stream->lastActivityTs = getTimeStampMS();

	// get stream's direction (from initiator / to initiator)
	bool isFromInitiator = tcp->getDstPort() == stream->localPort;
	TCPReassemblerDirStats* dirStats = (isFromInitiator) ? (&stats.fromInitiator) : (&stats.toInitiator);
	++dirStats->numPackets;

	// get the stream behind this direction
	TCPStreamDirection* streamDir = (isFromInitiator) ? (&stream->fromInitiator) : (&stream->toInitiator);

	// get the payload of this packet
	Payload payload = pkt->getTCP()->getPayload(pkt->getPayloadLength());


	stats.global.avgPayloadEmptySize.add(payload.length);
	dirStats->avgPayloadEmptySize.add(payload.length);

	// if we have a payload, reassemble and pass it to the listener
	if (payload.length != 0) {

		stats.global.avgPayloadSize.add(payload.length);
		dirStats->avgPayloadSize.add(payload.length);

		stats.global.sizePayload += payload.length;
		dirStats->sizePayload += payload.length;

		// syn with payload?
		if (tcp->isSyn()) {
			++stats.global.numSynPayload;
			++dirStats->numSynPayload;
		}

		// check and perform re-assembly
		bool ret = reAssemble(*streamDir, tcp->getSeqNumber(), payload, stream->listener);

		// reassembly failed? -> close stream
		if (!ret) {closeStream(stream); return;}

	} else {

		// adjust statistics
		++stats.global.numWithoutPayload;
		++dirStats->numWithoutPayload;
		if (tcp->getFlags() == TCP_FLAG_ACK) {
			++stats.global.numAckOnly;
			++dirStats->numAckOnly;
		}

	}

	// SYN-flag will increment seq-number by 1
	if (tcp->isSyn()) {++streamDir->nextSeqNumber;}

	// track number of FIN packets
	if (tcp->isFin()) {
		++stream->numFIN;
	}

	// if we have a 2xFIN or 1xRST flag, then close the stream
	// TODO (the ACK after the 2nd FIN will be added to "ignoredPacket" stats as the connection is already closed)
	if (stream->numFIN == 2 || tcp->isReset()) {
		debug(DBG_TCP, DBG_LVL_INFO, "connection closed: " << id);
		closeStream(stream);
	}

	// purge old connections that probably timed out
	purgeOld();


}

void TCPReassembler::purgeOld() {

	// for better performance, only call every x packets
	static uint32_t cnt = 0;
	if (++cnt < 64) {return;}
	cnt = 0;

	uint64_t curTs = getTimeStampMS();
	auto stream = streams.begin();
	while (stream != streams.end()) {
		uint64_t inactivity = curTs - stream->second->lastActivityTs;
		if (inactivity > timeoutMs) {
			auto toDelete = stream;
			++stream;
			closeStream(toDelete->second);
			++stats.numPurgedConnections;
		} else {
			++stream;
		}
	}

}

void TCPReassembler::closeStream(TCPStream* stream) {

	// inform stream-listener
	stream->listener->onClose();

	// cleanup
	streams.erase(stream->id);
	delete stream->listener;
	delete stream;

	// stats
	--stats.numOpenConnections;

}
