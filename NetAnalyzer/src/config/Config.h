/*
 * Config.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

#include "ConfigEntry.h"

#define DBG_CONFIG

// forward declarations
namespace tinyxml2 {
	class XMLDocument;
}

/**
 * provides access to the XML-configuration file
 */
class Config {

public:

	/** dtor */
	virtual ~Config();

	/** initialize the configuration */
	static void init(const std::string& file);

	/** destroy and cleanup */
	static void uninit();

	/** singleton access */
	static const ConfigEntry get();

protected:

	friend class ConfigEntry;

	/** an error occurred */
	static void onError(const std::string& msg);

	/** an error occurred */
	static void onError(const std::string& msg, int code);

private:

	/** hidden ctor */
	Config();

	/** the XML document */
	tinyxml2::XMLDocument* doc;

	/** singleton access */
	static Config* instance;

};

#endif /* CONFIG_H_ */
