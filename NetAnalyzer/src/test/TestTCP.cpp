/*
 * TestTCP.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#ifdef WITH_TESTS

#include "Test.h"
#include "../network/Packet.h"
#include "../network/processors/TCPReassembler.h"
#include "../network/processors/TCPConnectionListener.h"
#include "../network/processors/TCPStreamListener.h"
#include "../buffer/DataBuffer.h"

/** track correct state changes here */
struct TcpTestState {
	DataBuffer bufFrom;
	DataBuffer bufTo;
	bool open;
	TcpTestState() : open(false) {;}
};

/** class to listen for stream-data */
class StreamListener : public TCPStreamListener {
	TcpTestState& state;
public: StreamListener(TcpTestState& state) : state(state) {;}

void onOpen() {state.open = true;}
void onClose() {state.open = false;}
void onDataFromInitiator(const Payload& data)	{state.bufFrom.append(data.data, data.length);}
void onDataToInitiator(const Payload& data)		{state.bufTo.append(data.data, data.length);}

};

/** class to listen for connections */
class MyConListener : public TCPConnectionListener {

	StreamListener* listener;

public: MyConListener(StreamListener* listener) : listener(listener) {;}

TCPStreamListener* onTCPAccepted(uint16_t port) {(void) port; return listener;}

};


TEST(TCP, Reassemble) {


	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);
	TCPReassembler tr;
	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;


	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(40);

	// send syn
	pkt->getTCP()->setSeqNumber(1100);
	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setHeaderLength(20);
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	tr.onIPv4Packet(pkt);

	ASSERT_TRUE(state.open);
	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);

	// append 2 bytes of payload ('AB')
	data[40] = 'A';
	data[41] = 'B';
	pkt->getTCP()->setSeqNumber(1101);
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	tr.onIPv4Packet(pkt);

	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 2);

	// append 2 bytes of payload ('FG');
	data[40] = 'F';
	data[41] = 'G';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(1106);
	tr.onIPv4Packet(pkt);

	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 2);

	// append 3 bytes of payload ('FG');
	data[40] = 'C';
	data[41] = 'D';
	data[42] = 'E';
	pkt->setTotalLength(40+3);
	pkt->getTCP()->setSeqNumber(1103);
	tr.onIPv4Packet(pkt);

	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 7);

	// close
	pkt->setTotalLength(40);
	pkt->getTCP()->setSeqNumber(1108);
	pkt->getTCP()->setFlags(false, false, false, false, false, true);
	tr.onIPv4Packet(pkt);

	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 7);
	DataBufferData ret = state.bufFrom.get();
	ASSERT_DATA_EQ(ret.data, "ABCDEFG", 7);

}


/** payload within syn-ack packet? */
TEST(TCP, CheckNormal) {

	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);
	TCPReassembler tr;
	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;

	// send syn
	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(42);

	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setSrcPort(4444);
	pkt->getTCP()->setHeaderLength(20);

	// send SYN
	pkt->setTotalLength(40+0);
	pkt->getTCP()->setSeqNumber(0);
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	tr.onIPv4Packet(pkt);

	ASSERT_TRUE(state.open);
	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);


	// now: other direction
	pkt->getTCP()->setDstPort(4444);
	pkt->getTCP()->setSrcPort(80);

	// send SYN-ACK with payload
	data[40] = 'X';
	data[41] = 'Z';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(1000);
	pkt->getTCP()->setFlags(false, true, false, false, true, false);
	tr.onIPv4Packet(pkt);

	// check
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);
	ASSERT_EQ(state.bufTo.bytesUsed(), 2);

	// send another payload
	data[40] = 'B';
	data[41] = 'D';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(1003);
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	tr.onIPv4Packet(pkt);

	// check
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);
	ASSERT_EQ(state.bufTo.bytesUsed(), 4);
	DataBufferData ret = state.bufTo.get();
	ASSERT_DATA_EQ(ret.data, "XZBD", 4);

}

TEST(TCP, Retransmit) {

	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);
	TCPReassembler tr;
	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;

	// send syn
	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(40);

	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setHeaderLength(20);
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	tr.onIPv4Packet(pkt);

	// append 2 bytes of payload ('AB')
	data[40] = 'A';
	data[41] = 'A';
	pkt->getTCP()->setSeqNumber(1);
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	tr.onIPv4Packet(pkt);

	// send this packet twice
	tr.onIPv4Packet(pkt);

	// append 2 bytes of payload ('FG');
	data[40] = 'B';
	data[41] = 'B';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(3);
	tr.onIPv4Packet(pkt);

	// close
	pkt->setTotalLength(40);
	pkt->getTCP()->setSeqNumber(8);
	pkt->getTCP()->setFlags(false, false, false, false, false, true);
	tr.onIPv4Packet(pkt);

	DataBufferData ret = state.bufFrom.get();
	ASSERT_DATA_EQ(ret.data, "AABB", 4);

}


/** send to many out-of-order packets. should not produce any mem-leaks */
TEST(TCP, BufferOverflow) {

	TCPReassembler tr;
	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);

	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;

	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setHeaderLength(20);

	ASSERT_FALSE(state.open);

	// syn
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	pkt->getTCP()->setSeqNumber(1000);
	tr.onIPv4Packet(pkt);

	ASSERT_TRUE(state.open);

	// send some non-continous packets
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	for (uint32_t i = 0; i < 128; ++i) {
		pkt->getTCP()->setSeqNumber(1001 + 100 + i*2);
		tr.onIPv4Packet(pkt);
	}

	ASSERT_FALSE(state.open);

}

/** payload within syn packet? */
TEST(TCP, SynPayload) {

	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);
	TCPReassembler tr;
	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;

	// send syn
	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(42);

	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setHeaderLength(20);

	// send SYN
	data[40] = 'A';
	data[41] = 'B';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(0);
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	tr.onIPv4Packet(pkt);

	ASSERT_TRUE(state.open);
	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 2);

	// append 2 bytes of payload ('AB')
	data[40] = 'C';
	data[41] = 'D';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(3);
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	tr.onIPv4Packet(pkt);

	// check
	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 4);

	DataBufferData ret = state.bufFrom.get();
	ASSERT_DATA_EQ(ret.data, "ABCD", 4);

}

/** payload within syn-ack packet? */
TEST(TCP, SynAckPayload) {

	TcpTestState state;
	StreamListener* streamListener = new StreamListener(state);
	MyConListener listener(streamListener);
	TCPReassembler tr;
	tr.setConnectionListener(&listener);

	uint8_t data[2048] = {0};
	PacketIPv4* pkt = (PacketIPv4*) &data;

	// send syn
	pkt->setProtocol(IPv4_PAYLOAD_TCP);
	pkt->setHeaderLength(20);
	pkt->setTotalLength(42);

	pkt->getTCP()->setDstPort(80);
	pkt->getTCP()->setSrcPort(4444);
	pkt->getTCP()->setHeaderLength(20);

	// send SYN
	pkt->setTotalLength(40+0);
	pkt->getTCP()->setSeqNumber(0);
	pkt->getTCP()->setFlags(false, false, false, false, true, false);
	tr.onIPv4Packet(pkt);

	ASSERT_TRUE(state.open);
	ASSERT_EQ(state.bufTo.bytesUsed(), 0);
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);


	// now: other direction
	pkt->getTCP()->setDstPort(4444);
	pkt->getTCP()->setSrcPort(80);

	// send SYN-ACK with payload
	data[40] = 'X';
	data[41] = 'Z';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(1000);
	pkt->getTCP()->setFlags(false, true, false, false, true, false);
	tr.onIPv4Packet(pkt);

	// check
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);
	ASSERT_EQ(state.bufTo.bytesUsed(), 2);

	// send another payload
	data[40] = 'B';
	data[41] = 'D';
	pkt->setTotalLength(40+2);
	pkt->getTCP()->setSeqNumber(1003);
	pkt->getTCP()->setFlags(false, false, false, false, false, false);
	tr.onIPv4Packet(pkt);

	// check
	ASSERT_EQ(state.bufFrom.bytesUsed(), 0);
	ASSERT_EQ(state.bufTo.bytesUsed(), 4);
	DataBufferData ret = state.bufTo.get();
	ASSERT_DATA_EQ(ret.data, "XZBD", 4);

}

#endif
