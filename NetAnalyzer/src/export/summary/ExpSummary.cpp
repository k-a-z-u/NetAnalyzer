/*

 * ExpSummary.cpp
 *
 *  Created on: May 2, 2013
 *      Author: kazu
 */

#include "ExpSummary.h"
#include "../../lib/tinyxml2/tinyxml2.h"
#include "../../Helper.h"
#include "../../analyzer/Summary.h"
#include "../../network/processors/TCPReassembler.h"
#include "../../http/HTTPDecoderStats.h"

#include <sstream>


ExpSummary::ExpSummary(PeriodicExporterThread* thread) : PeriodicExporter(thread) {

	// create
	doc = new tinyxml2::XMLDocument();

	// where to store to
	dstFile = File(Exporter::folder, "summary.xml");

	// create root-node
	root = doc->NewElement("summary");
	doc->InsertEndChild(root);

}

ExpSummary::~ExpSummary() {

	// cleanup
	delete doc;

}

/** -------------------------------- HELPER FUNCTIONS -------------------------------- */

void ExpSummary::addNodeWithValue(tinyxml2::XMLElement* parent, const std::string& nodeName, const std::string& nodeValue) {
	tinyxml2::XMLElement* tmpNode = doc->NewElement(nodeName.c_str());
	tinyxml2::XMLText* tmpText = doc->NewText(nodeValue.c_str());
	tmpNode->InsertEndChild(tmpText);
	parent->InsertEndChild(tmpNode);
}

void ExpSummary::addNodeWithValue(tinyxml2::XMLElement* parent, const std::string& nodeName, long nodeValue) {
	std::stringstream ss;
	ss << nodeValue;
	addNodeWithValue(parent, nodeName, ss.str());
}

void ExpSummary::setAttribute(tinyxml2::XMLElement* parent, const std::string& attrName, long attrValue) {
	std::stringstream ss;
	ss << attrValue;
	parent->SetAttribute(attrName.c_str(), ss.str().c_str());
}

void ExpSummary::appendAll() {

	// the node encapsulating all entries for this timestamp
	tinyxml2::XMLElement* node = doc->NewElement("entry");
	setAttribute(node, "ts", getTimeStampMS());
	root->InsertEndChild(node);

	appendTCP(node);
	appendHTTP(node);

}


/** -------------------------------- TCP -------------------------------- */

void ExpSummary::appendTCPDir(tinyxml2::XMLElement* parent, const TCPReassemblerDirStats& dir){

	addNodeWithValue(parent, "packets", dir.numPackets);
	addNodeWithValue(parent, "errors", dir.numErrors);

	addNodeWithValue(parent, "retransmits", dir.numRetransmits);
	addNodeWithValue(parent, "seqMismatch", dir.numSeqNumberMismatch);
	addNodeWithValue(parent, "seqCorrected", dir.numSeqNumberReOrdered);

	addNodeWithValue(parent, "withoutPayload", dir.numWithoutPayload);
	addNodeWithValue(parent, "ackOnly", dir.numAckOnly);
	addNodeWithValue(parent, "synPayload", dir.numSynPayload);

	addNodeWithValue(parent, "payload", dir.sizePayload);
	addNodeWithValue(parent, "avgPerPacket", dir.avgPayloadSize.getAverage());
	addNodeWithValue(parent, "avgPerPacketIncludingEmpty", dir.avgPayloadEmptySize.getAverage());

}

void ExpSummary::appendTCP(tinyxml2::XMLElement* parent) {

	// sanity check
	if (sum->tcp.stats == nullptr) {return;}

	// create node for TCP
	tinyxml2::XMLElement* tcp = doc->NewElement("tcp");
	parent->InsertEndChild(tcp);

	addNodeWithValue(tcp, "connections", sum->tcp.stats->numConnections);
	addNodeWithValue(tcp, "ignoredPackets", sum->tcp.stats->numPacketsIgnored);
	addNodeWithValue(tcp, "idCollisions", sum->tcp.stats->numIDCollision);

	// create sub-nodes for "to-initiator" "from-initiator" and "global"
	tinyxml2::XMLElement* nodeGlobal = doc->NewElement("global");
	appendTCPDir(nodeGlobal, sum->tcp.stats->global);
	tcp->InsertEndChild(nodeGlobal);

	tinyxml2::XMLElement* nodeToInit = doc->NewElement("toInitiator");
	appendTCPDir(nodeToInit, sum->tcp.stats->toInitiator);
	tcp->InsertEndChild(nodeToInit);

	tinyxml2::XMLElement* nodeFromInit = doc->NewElement("fromInitiator");
	appendTCPDir(nodeFromInit, sum->tcp.stats->fromInitiator);
	tcp->InsertEndChild(nodeFromInit);

}

/** -------------------------------- HTTP -------------------------------- */

void ExpSummary::appendHTTPDecoder(tinyxml2::XMLElement* parent, const HTTPDecoderStatsDir& dir) {

	addNodeWithValue(parent, "total", dir.numHeaders);
	addNodeWithValue(parent, "ok", dir.numOK);

	addNodeWithValue(parent, "normal", dir.numNormal);
	addNodeWithValue(parent, "chunked", dir.numChunked);
	addNodeWithValue(parent, "compressed", dir.numCompressed);
	addNodeWithValue(parent, "decompressErrors", dir.numDecompressErrors);

	addNodeWithValue(parent, "withoutPayload", dir.numWithoutPayload);

	addNodeWithValue(parent, "avgHeaderSize", dir.avgHeaderSize.getAverage());

}

void ExpSummary::appendHTTPStatsCompr(tinyxml2::XMLElement* parent, const std::string& name, const CompressorStats& stats) {

	// create node for this compressor
	tinyxml2::XMLElement* comp = doc->NewElement("compressor");
	comp->SetAttribute("type", name.c_str());
	parent->InsertEndChild(comp);

	// add details
	addNodeWithValue(comp, "compressedSize", stats.compression.compressedPayloadSize);
	addNodeWithValue(comp, "usedTimeUS", stats.compression.compressedPayloadUs);

}

void ExpSummary::appendHTTPStats(tinyxml2::XMLElement* parent, const HTTPStats& stats) {


	addNodeWithValue(parent, "num", stats.numHeaders);
	addNodeWithValue(parent, "headerSize", stats.sizeHeaders);
	addNodeWithValue(parent, "payloadSize", stats.sizePayload);

	// details by HTTP code
	tinyxml2::XMLElement* codes = doc->NewElement("codes");
	parent->InsertEndChild(codes);

	addNodeWithValue(codes, "200", stats.numHttpOK);
	addNodeWithValue(codes, "301", stats.numHttpMoved);
	addNodeWithValue(codes, "304", stats.numHttpNotModified);
	addNodeWithValue(codes, "404", stats.numHttpNotFound);
	addNodeWithValue(codes, "500", stats.numHttpIntError);
	addNodeWithValue(codes, "other", stats.numHttpOther);

	// compression node
	tinyxml2::XMLElement* compr = doc->NewElement("compression");
	parent->InsertEndChild(compr);

	// append stats for each compressor
	for (const auto& c : stats.comprStats) {appendHTTPStatsCompr(compr, c.first, c.second);}

}


void ExpSummary::appendHTTP(tinyxml2::XMLElement* parent) {

	// sanity check
	if (sum->HTTP.stats == nullptr) {return;}

	// create node for HTTP
	tinyxml2::XMLElement* http = doc->NewElement("http");
	parent->InsertEndChild(http);


	// append decoder information
	tinyxml2::XMLElement* httpDec = doc->NewElement("decoder");
	http->InsertEndChild(httpDec);

	tinyxml2::XMLElement* httpDecReq = doc->NewElement("requests");
	httpDec->InsertEndChild(httpDecReq);
	appendHTTPDecoder(httpDecReq, sum->HTTP.stats->requests);

	tinyxml2::XMLElement* httpDecResp = doc->NewElement("responses");
	httpDec->InsertEndChild(httpDecResp);
	appendHTTPDecoder(httpDecResp, sum->HTTP.stats->responses);


	// append stats information
	tinyxml2::XMLElement* httpStats = doc->NewElement("stats");
	http->InsertEndChild(httpStats);


	// global stats
	tinyxml2::XMLElement* httpStatsGlobal = doc->NewElement("global");
	httpStats->InsertEndChild(httpStatsGlobal);

	tinyxml2::XMLElement* httpStatsReq = doc->NewElement("requests");
	httpStatsGlobal->InsertEndChild(httpStatsReq);
	appendHTTPStats(httpStatsReq, sum->HTTP.request);

	tinyxml2::XMLElement* httpStatsResp = doc->NewElement("responses");
	httpStatsGlobal->InsertEndChild(httpStatsResp);
	appendHTTPStats(httpStatsResp, sum->HTTP.response);


	// stats by content type
	for (auto& ct : sum->HTTP.byContentType) {

		// create node
		tinyxml2::XMLElement* sub = doc->NewElement("contentType");
		sub->SetAttribute("type", ct.first.c_str());
		httpStats->InsertEndChild(sub);

		// create node for all responses
		tinyxml2::XMLElement* subResp = doc->NewElement("responses");
		sub->InsertEndChild(subResp);

		// split responses in 3 sub nodes
		tinyxml2::XMLElement* subRespTot = doc->NewElement("total");
		subResp->InsertEndChild(subRespTot);
		tinyxml2::XMLElement* subRespStat = doc->NewElement("static");
		subResp->InsertEndChild(subRespStat);
		tinyxml2::XMLElement* subRespDyn = doc->NewElement("dynamic");
		subResp->InsertEndChild(subRespDyn);

		// append stats to each sub-node
		appendHTTPStats(subRespTot, ct.second.response);
		appendHTTPStats(subRespStat, ct.second.responseStatic);
		appendHTTPStats(subRespDyn, ct.second.responseDynamic);

	}

}

void ExpSummary::exportMe() {

	// append all nodes
	appendAll();

	// save
	doc->SaveFile( dstFile.getAbsolutePath().c_str(), false );

}
