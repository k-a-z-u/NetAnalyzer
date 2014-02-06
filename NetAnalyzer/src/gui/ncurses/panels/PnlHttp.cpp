/*
 * PnlHttp.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#include "PnlHttp.h"
#include "../../../analyzer/Summary.h"
#include "../../../http/HTTPDecoderStats.h"

#include "../elements/GPopUp.h"
#include "../helper/GuiHelper.h"

/**
 * additional HTTP details within a PopUp window
 */
class HttpPopUp : public GPanel {

public:

	const Summary& sum;

	/** ctor */
	HttpPopUp(const Summary& sum) : GPanel(), sum(sum) {;}

	bool onKey(int key) override { (void) key; return false; }

	void printStats(GRender& render, uint32_t x, const HTTPDecoderStatsDir& stats) {

		GuiHelper::printInt(render, x, 3, stats.avgHeaderSize.getAverage());

	}

	void refresh(GRender& render) override {

		// skip if no stats available
		if (sum.HTTP.stats == nullptr) {return;}

		const uint32_t x0 = 1;

		// header
		render.printR(36+22*0, 1, "request");
		render.printR(36+22*1, 1, "response");

		// legend
		render.print(x0, 3, "average header size:");

		// print stats
		printStats(render, 36+22*0, sum.HTTP.stats->requests);
		printStats(render, 36+22*1, sum.HTTP.stats->responses);

	}

};




PnlHttp::PnlHttp(const Summary& sum) :
GPanel(), sum(sum) {
}

PnlHttp::~PnlHttp() {
	;
}

bool PnlHttp::onKey(int key) {
	static HttpPopUp popUp(sum);
	if (key == '\n') {new GPopUp("HTTP-Details", popUp);}
	return false;
}

void PnlHttp::refresh(GRender& render) {

	// check if stats available
	if (sum.HTTP.stats == nullptr) {return;}

	render.print(12, 0, "Request");
	render.print(24, 0, "Response");

	render.print(1,  1, "number:");
	render.print(11, 1, intToString(sum.HTTP.stats->requests.numHeaders));
	render.print(23, 1, intToString(sum.HTTP.stats->responses.numHeaders));

	render.print(1,  2, "compr:");
	render.print(11, 2, intToString(sum.HTTP.stats->requests.numCompressed));
	render.print(23, 2, intToString(sum.HTTP.stats->responses.numCompressed));

	render.print(1,  3, "chunked:");
	render.print(11, 3, intToString(sum.HTTP.stats->requests.numChunked));
	render.print(23, 3, intToString(sum.HTTP.stats->responses.numChunked));


	render.print(1,  4, "OK:");
	render.print(11, 4, intToString(sum.HTTP.stats->requests.numOK));
	render.print(23, 4, intToString(sum.HTTP.stats->responses.numOK));



	render.print(1,  5, "header:");
	render.print(11, 5, intToString(sum.HTTP.request.sizeHeaders));
	render.print(23, 5, intToString(sum.HTTP.response.sizeHeaders));

	render.print(1,  6, "payload:");
	render.print(11, 6, intToString(sum.HTTP.request.sizePayload));
	render.print(23, 6, intToString(sum.HTTP.response.sizePayload));

}

