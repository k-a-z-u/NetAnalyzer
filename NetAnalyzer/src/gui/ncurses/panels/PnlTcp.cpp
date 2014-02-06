/*
 * PnlTcp.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#include "PnlTcp.h"

#include "../elements/GPopUp.h"
#include "../../../analyzer/Summary.h"
#include "../../../network/processors/TCPReassembler.h"

#include "../helper/GuiHelper.h"

/**
 * additional TCP details within a PopUp window
 */
class TcpPopUp : public GPanel {

public:

	const Summary& sum;

	/** ctor */
	TcpPopUp(const Summary& sum) : GPanel(), sum(sum) {;}

	bool onKey(int key) override { (void) key; return false; }

	void printStats(GRender& render, uint32_t x, const TCPReassemblerDirStats& stats) {


		GuiHelper::printInt(render, x, 8, stats.numPackets);
		GuiHelper::printInt(render, x, 9, stats.numErrors);

		GuiHelper::printInt(render, x, 11, stats.numRetransmits);
		GuiHelper::printInt(render, x, 12, stats.numSeqNumberMismatch);
		GuiHelper::printInt(render, x, 13, stats.numSeqNumberReOrdered);

		GuiHelper::printIntPercent(render, x, 15, stats.numPackets, stats.numWithoutPayload, "");
		GuiHelper::printIntPercent(render, x, 16, stats.numPackets, stats.numAckOnly, "");
		GuiHelper::printIntPercent(render, x, 17, stats.numPackets, stats.numSynPayload, "");

		GuiHelper::printInt(render, x, 19, stats.sizePayload);
		GuiHelper::printFloat(render, x, 20, stats.sizePayload / 1024.0f / (getUptimeMS()/1000) , " kB/s");
		GuiHelper::printInt(render, x, 21, stats.avgPayloadSize.getAverage());
		GuiHelper::printInt(render, x, 22, stats.avgPayloadEmptySize.getAverage());

	}

	void refresh(GRender& render) override {

		// skip if no stats available
		if (sum.tcp.stats == nullptr) {return;}

		const uint32_t x0 = 1;

		// header
		render.printR(36+22*0, 7, "global");
		render.printR(36+22*1, 7, "from initiator");
		render.printR(36+22*2, 7, "to initiator");

		// legend
		render.print(x0, 1, "connctions (current):");
		render.print(x0, 2, "connections (total):");
		render.print(x0, 3, "connections purged:");
		render.print(x0, 4, "ignored packets:");
		render.print(x0, 5, "id collisions");

		render.print(x0, 8, "processed packets:");
		render.print(x0, 9, "errors:");

		render.print(x0, 11, "retransmits:");
		render.print(x0, 12, "out-of-seq:");
		render.print(x0, 13, "corrected:");

		render.print(x0, 15, "flag-only packets:");
		render.print(x0, 16, "ACK-only packets:");
		render.print(x0, 17, "SYN payload:");

		render.print(x0, 19, "payload:");
		render.print(x0, 20, "payload rate:");
		render.print(x0, 21, "per packet:");
		render.print(x0, 22, "per packet(empty):");

		// global values
		GuiHelper::printInt(render, 36, 1, sum.tcp.stats->numOpenConnections);
		GuiHelper::printInt(render, 36, 2, sum.tcp.stats->numConnections);
		GuiHelper::printInt(render, 36, 3, sum.tcp.stats->numPurgedConnections);
		GuiHelper::printInt(render, 36, 4, sum.tcp.stats->numPacketsIgnored);
		GuiHelper::printInt(render, 36, 5, sum.tcp.stats->numIDCollision);

		printStats(render, 36+22*0, sum.tcp.stats->global);
		printStats(render, 36+22*1, sum.tcp.stats->fromInitiator);
		printStats(render, 36+22*2, sum.tcp.stats->toInitiator);

	}

};




PnlTcp::PnlTcp(const Summary& sum) :
				GPanel(), sum(sum) {
}

PnlTcp::~PnlTcp() {
	;
}

bool PnlTcp::onKey(int key) {
	static TcpPopUp popUp(sum);
	if (key == '\n') {new GPopUp("TCP-Details", popUp);}
	return false;
}

void PnlTcp::refresh(GRender& render) {

	// skip if no stats available
	if (sum.tcp.stats == nullptr) {return;}

	render.print(1, 0, "processed:");
	render.print(1, 1, "ignored:");
	render.print(1, 2, "retransmits:");
	render.print(1, 3, "seq-mismatch:");
	render.print(1, 4, "seq-corrected:");
	render.print(1, 5, "errors:");
	render.print(1, 6, "connections:");

	render.print(17, 0, intToString(sum.tcp.stats->global.numPackets) );
	render.print(17, 1, intToString(sum.tcp.stats->numPacketsIgnored) );
	render.print(17, 2, intToString(sum.tcp.stats->global.numRetransmits) );
	render.print(17, 3, intToString(sum.tcp.stats->global.numSeqNumberMismatch) );
	render.print(17, 4, intToString(sum.tcp.stats->global.numSeqNumberReOrdered) );
	render.print(17, 5, intToString(sum.tcp.stats->global.numErrors) );
	render.print(17, 6, intToString(sum.tcp.stats->numConnections) );

}
