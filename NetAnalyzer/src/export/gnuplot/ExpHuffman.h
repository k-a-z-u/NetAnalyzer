/*
 * ExpHuffman.h
 *
 *  Created on: Apr 21, 2013
 *      Author: kazu
 */

#ifndef EXPHUFFMAN_H_
#define EXPHUFFMAN_H_

#include <string>
#include <map>
#include <iosfwd>

#include "../PeriodicExporter.h"

/**
 * export huffman-trees for specific content types every x seconds to file
 */
class ExpHuffman : public PeriodicExporter {

public:

	/** ctor */
	ExpHuffman(PeriodicExporterThread* thread);

	/** dtor */
	virtual ~ExpHuffman();

	/** add one content-type to export the tree for */
	void addContentType(const std::string& type);

private:

	/** hidden copy ctor */
	ExpHuffman(const ExpHuffman&);

	void exportMe() override;

	std::map<std::string, std::ofstream*> contentTypes;

};

#endif /* EXPHUFFMAN_H_ */
