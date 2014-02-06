/*
 * PnlHttpDetails.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#include "PnlHttpDetails.h"
#include "../../../analyzer/Summary.h"
#include "PnlContentType.h"
#include "../elements/GPopUp.h"


#include <vector>


class ContentTypeEntry : public GListEntry {

public:

	const HTTPContentType& data;

	ContentTypeEntry(const HTTPContentType& data) : data(data) {;}

	std::string getValue(uint32_t column) override {

		switch(column) {

		case 0:	return data.contentType;
		case 1:	return intToString(data.response.numHeaders);
		case 2:
			if (data.response.numHeaders == 0) {return "";}
			return intToString(data.response.sizeHeaders) +\
					" (" + intToString(data.response.sizeHeaders / data.response.numHeaders) + ")";
		case 3:
			if (data.response.numHeaders == 0) {return "";}
			return intToString(data.response.sizePayload) +\
					" (" + intToString(data.response.sizePayload / data.response.numHeaders) + ")";
		case 4: {
			uint32_t latencyPerTransfer = data.response.latency.getAvg();
			return intToString(latencyPerTransfer) + " ms";}
//		case 5: {
//			uint64_t huffmanCompSize = data.huffman.getCompressedSize();
//			uint32_t huffmanPercent = huffmanCompSize * 100l / data.response.sizePayload;
//			//uint32_t huffmanPercent = data.huffman.getCompressionRate();
//			return intToString(huffmanCompSize) + " (" + intToString(huffmanPercent) + "%)";}

		default: return "";

		}
	}

};




PnlHttpDetails::PnlHttpDetails(const Summary& sum) :
GPanel(), sum(sum), list() {

	list.addColumn("Content-Type", 22);
	list.addColumn("#Req", 6);
	list.addColumn("Header", 9+6);
	list.addColumn("Payload", 11+10);
	list.addColumn("Latency", 8);
	//list.addColumn("HUFFMAN", 13);

	list.addListener(this);

}

PnlHttpDetails::~PnlHttpDetails() {

}


void PnlHttpDetails::onSelectedItem(GListEntry* entry) {
	static PnlContentType* type = new PnlContentType();
	const ContentTypeEntry* e = (ContentTypeEntry*) entry;
	type->setContentType( (e->data) );
	new GPopUp("Content-Type " + e->data.contentType, *type);
}


bool PnlHttpDetails::onKey(int key) {
	bool ret = list.onKey(key);
	return ret;
}


void PnlHttpDetails::refresh(GRender& render) {

	// push the whole map into a (sortable) vector
	//std::map<std::string&, struct HTTPContentType&>::const_iterator it;
	//for(it = sum.HTTP.byContentType.begin(); it != sum.HTTP.byContentType.end(); ++it) {

	for (const auto& it : sum.HTTP.byContentType) {

		// already contained within list??
		bool found = false;
		std::vector<GListEntry*>::const_iterator itList;
		for ( const auto& itList : list.getEntries() ) {
			ContentTypeEntry* e = (ContentTypeEntry*) (itList);
			if (&e->data == &it.second) {found = true; break;}
		}

		// not present, add to list
		if (!found) {
			ContentTypeEntry* e = new ContentTypeEntry( it.second );
			list.addEntry(e);
		}
	}


	// sort the entries in the list
	auto lambda = [] (const GListEntry* a, const GListEntry* b) -> int {
		return ((ContentTypeEntry*)a)->data.response.sizePayload > ((ContentTypeEntry*)b)->data.response.sizePayload;
	};
	list.sort(lambda);


	// display the list
	list.setBounds(b.x, b.y, b.w, b.h);
	list.render(render);

}

