/*
 * Exporter.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: kazu
 */


#include "Exporter.h"
#include "../misc/File.h"

Exporter::Exporter() : sum(nullptr) {
	;
}

void Exporter::setModel(const Summary& sum) {
	this->sum = &sum;
}


/** statics */
File Exporter::folder = File();

void Exporter::setFolder(File folder) {
	Exporter::folder = folder;
}
