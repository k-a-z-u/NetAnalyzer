/*
 * PnlGlobal.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#include "PnlGlobal.h"
#include "../../../analyzer/Summary.h"
#include "../elements/GPopUp.h"

/**
 * pop-up within global-panel to show some memory-stats etc.
 */
#include <malloc.h>
class GlobalPopUp : public GPanel {

public:

	const Summary& sum;

	/** ctor */
	GlobalPopUp(const Summary& sum) : GPanel(), sum(sum) {;}

	bool onKey(int key) override { (void) key; return false; }

	void refresh(GRender& render) override {

		render.print(1, 1, "Memory");
		render.print(1, 2, "total:");
		render.print(1, 3, "used:");

		// load memory stats
		struct mallinfo m = mallinfo();
		render.print(14, 2, intToString( (uint64_t)m.fordblks + (uint64_t)m.uordblks ) + " Bytes");
		render.print(14, 3, intToString( (uint64_t)m.uordblks) + " Bytes");

	}


};




PnlGlobal::PnlGlobal(const Summary& sum) : GPanel(), sum(sum) {
}

PnlGlobal::~PnlGlobal() {
	// TODO Auto-generated destructor stub
}

bool PnlGlobal::onKey(int key) {
	static GlobalPopUp popUp(sum);
	if (key == '\n') {new GPopUp("Global-Details", popUp);}
	return false;
}

void PnlGlobal::refresh(GRender& render) {

	std::string time = getTimeFormatted(getUptimeMS());

	render.print(26, 0, time.c_str());

	render.print(1, 0, "packets:");
	render.print(12, 0, intToString(sum.receivedPackets));

	render.print(1, 1, "bytes:");
	render.print(12, 1, intToString(sum.receivedBytes));


	render.print(12, 3, "Total");
	render.print(25, 3, "PerPacket");

	uint32_t payload = sum.receivedBytes - sum.receivedProtocolOverheadBytes;
	uint32_t payloadPerPacket = payload / (sum.receivedPackets + 0.00001);
	render.print(1, 4, "payload:");
	render.print(12, 4, intToString(payload).c_str());
	render.print(25, 4, intToString(payloadPerPacket).c_str());


	uint32_t protoOverhead = (sum.receivedProtocolOverheadBytes) / (float) (payload + 0.00001) * 100;
	uint32_t overheadPerPacket = sum.receivedProtocolOverheadBytes / (sum.receivedPackets + 0.00001);
	std::string overhead = "(" + floatToString(protoOverhead, 1) + "%)";
	render.print(1, 5, "overhead:");
	render.print(12, 5, intToString(sum.receivedProtocolOverheadBytes).c_str());
	render.print(25, 5, intToString(overheadPerPacket).c_str());
	render.print(29, 5, overhead.c_str());

}
