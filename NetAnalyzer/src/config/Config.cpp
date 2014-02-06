/*
 * Config.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#include "Config.h"
#include "../lib/tinyxml2/tinyxml2.h"

#include <iostream>
#include <string>
#include <exception>

class ConfigException : public std::exception {
public:
	ConfigException(const std::string& msg) : msg(msg) {;}
	ConfigException(const std::string& msg, int code) {
		this->msg = msg + "\ncode:" + std::to_string(code);
	}
	const char* what() const throw() {return msg.c_str();}
private:
	std::string msg;
};


Config::Config() : doc(nullptr) {

}

Config::~Config() {
	doc->Clear();
	delete doc;
}

const ConfigEntry Config::get() {
	if (Config::instance == nullptr) {
		Config::onError("init() config first!");
	}
	tinyxml2::XMLNode* node = Config::instance->doc->FirstChildElement("NetAnalyzer");
	
	// setup a dummy entry as root to ensure the config is "setup" even when loading the file crashes
	if (node == nullptr) {
		tinyxml2::XMLElement* el = Config::instance->doc->NewElement("NetAnalyzer");
		Config::instance->doc->InsertEndChild(el);
		node = el;
	}
	
	return ConfigEntry( (tinyxml2::XMLElement*) node );
}

void Config::init(const std::string& fileName) {

	if (Config::instance != nullptr) {
		Config::onError("config already initialized!");
	}

	Config::instance = new Config();
	Config::instance->doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLError ret = Config::instance->doc->LoadFile(fileName.c_str());

	

	// check return code
	switch (ret) {
	case tinyxml2::XML_NO_ERROR: break;
	case tinyxml2::XML_ERROR_FILE_NOT_FOUND:		Config::onError("could not find config file: '" + fileName + "'"); break;
	case tinyxml2::XML_ERROR_PARSING:				Config::onError("parsing error while loading config file"); break;
	case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:	Config::onError("parsing error while loading config file: mismatched element found"); break;
	default:										Config::onError("unknown error while loading config file", ret);
	}

	// sanity check
	if (Config::instance->doc->FirstChildElement("NetAnalyzer") == nullptr) {
		Config::onError("config file needs a root named 'NetAnalyzer'!");
	}

}

void Config::uninit() {
	if (Config::instance != nullptr) {
		delete Config::instance;
		Config::instance = nullptr;
	}
}

void Config::onError(const std::string& msg) {
	//std::cerr << "ConfigError:" << std::endl;
	//std::cerr << msg << std::endl;
	//exit(100);
	throw ConfigException(msg);
}

void Config::onError(const std::string& msg, int code) {
	//std::cerr << "ConfigError:" << std::endl;
	//std::cerr << msg << std::endl;
	//std::cerr << "code: " << code << std::endl;
	exit(100);
	throw ConfigException(msg, code);
}


/** init singleton */
Config* Config::instance = nullptr;


