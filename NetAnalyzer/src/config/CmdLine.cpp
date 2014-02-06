/*
 * CmdLine.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#include "CmdLine.h"
#include <iostream>






CmdLine::CmdLine(int argc, char* argv[], const std::string& usage) : usage(usage) {
	append(argc, argv);
}

CmdLine::~CmdLine() {

}


void CmdLine::append(int argc, char* argv[]) {

	// sanity check:
	if (argc == 1) {
		printUsage(argv[0]);
		exit(0);
	}

	// append all params to vector
	for (int i = 1; i < argc; ++i) {

		std::string arg = argv[i];

		// format: --dev=wlan0
		size_t pos1 = arg.find("--");
		size_t pos2 = arg.find('=');
		if (pos1 == 0) {

			// --dev=XYZ or --dev ??
			if (pos2 != std::string::npos) {
				std::string key = arg.substr(2, pos2-2);
				std::string val = arg.substr(pos2+1);
				params.push_back(CmdKeyVal(key, val));
			} else {
				std::string key = arg.substr(2);
				std::string val = "";
				params.push_back(CmdKeyVal(key, val));
			}

		}


	}

}

bool CmdLine::has(const std::string& name) const {

	// find "name" within params
	for (const auto& param : params) {
		if (param.key.compare(name) == 0) {return true;}
	}

	// not found
	return false;

}

void CmdLine::printUsage(const std::string& progName) {
	std::cout << "incorrect command-line. use like this:" << std::endl;
	std::cout << progName << " " << usage << std::endl;
}

const std::string& CmdLine::get(const std::string& name, bool mandatory, const std::string& fallback) const {

	// find "name"
	for (uint32_t i = 0; i < params.size(); ++i) {

		// name found? -> return value
		if (params[i].key.compare(name) == 0) {return params[i].val;}

	}

	// key not found but was not mandatory -> return fallback (or empty-string if fallback was not defined)
	if (!mandatory) {return fallback;}

	// missing mandatory key
	std::cerr << "commandline MUST specify '" << name << "'!" << std::endl;
	exit(100);

}
