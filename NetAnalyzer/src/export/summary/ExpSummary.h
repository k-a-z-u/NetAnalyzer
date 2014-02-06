/*
 * ExpSummary.h
 *
 *  Created on: May 2, 2013
 *      Author: kazu
 */

#ifndef EXPSUMMARY_H_
#define EXPSUMMARY_H_

#include "../PeriodicExporter.h"
#include "../../misc/File.h"


// forward declarations
namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
}

struct CompressorStats;
struct HTTPStats;
struct HTTPDecoderStatsDir;
struct TCPReassemblerDirStats;


/**
 * export all summary-facts as XML
 */
class ExpSummary : public PeriodicExporter {

public:

	/** ctor */
	ExpSummary(PeriodicExporterThread* thread);

	/** dtor */
	virtual ~ExpSummary();

private:

	void exportMe() override;

	/** append all entries */
	void appendAll();


	/** append tcp-details to this node */
	void appendTCP(tinyxml2::XMLElement* parent);

	/** append tcp-directional stats */
	void appendTCPDir(tinyxml2::XMLElement* parent, const TCPReassemblerDirStats& dir);


	/** append http details to the given node */
	void appendHTTP(tinyxml2::XMLElement* parent);

	/** append http-decoder-details to the given node */
	void appendHTTPDecoder(tinyxml2::XMLElement* parent, const HTTPDecoderStatsDir& dir);

	/** append http-stats to the given node */
	void appendHTTPStats(tinyxml2::XMLElement* parent, const HTTPStats& stats);

	/** append compressor-stats to the given node */
	void appendHTTPStatsCompr(tinyxml2::XMLElement* parent, const std::string& name, const CompressorStats& stats);


	/** append a new node with the given value */
	void addNodeWithValue(tinyxml2::XMLElement* parent, const std::string& nodeName, const std::string& nodeValue);

	/** append a new node with the given value */
	void addNodeWithValue(tinyxml2::XMLElement* parent, const std::string& nodeName, long nodeValue);

	/** attach an attribute to the given node */
	void setAttribute(tinyxml2::XMLElement* node, const std::string& attrName, long attrValue);

	/** the XML-document to write to */
	tinyxml2::XMLDocument* doc;

	/** the root-node */
	tinyxml2::XMLElement* root;

	/** where to save xml to */
	File dstFile;

};

#endif /* EXPSUMMARY_H_ */
