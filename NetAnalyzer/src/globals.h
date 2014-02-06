/*
 * globals.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "gui/ncurses/GUINcurses.h"
#include "gui/GUIDummy.h"

#include "analyzer/HTTPAnalyzer.h"

class PacketProvider;
class Analyzer;
class PeriodicExporterThread;
class HTTPDataSource;

#include "compression/dict/DictFlex.h"
#include "compression/lzwDict/LZWDict.h"
#include "export/Exporter.h"



#define DBG_MAIN		"MAIN"


/* global variables. they must be accessible from siginthandler */
struct globals {

	/** the name of the gui we will use (ncurses, none, ...) */
	std::string guiName = "";

	/** the gui to use */
	GUI* gui = nullptr;

	/** the packet source (sniffer, file, ...) when using NetSource as provider */
	PacketProvider* packetProvider = nullptr;

	/** the element providing http-data to the analyzer (e.g. NetSource or FolderSource) */
	HTTPDataSource* httpSrc = nullptr;

	/** this is where the magic happens */
	Analyzer* analyzer = nullptr;

	/** the thread to trigger all periodic exporters */
	PeriodicExporterThread* expThread = nullptr;

	/** all exporters to use */
	std::vector<std::unique_ptr<Exporter>> exporters;

	/** all loaded dictionaries */
	std::vector<std::unique_ptr<DictFlex>> dicts;

	/** all loaded dictionaries */
	std::vector<std::unique_ptr<LZWDict>> lzwDicts;

	/** let the main sleep until this is false */
	bool running = true;

};


#endif /* GLOBALS_H_ */
