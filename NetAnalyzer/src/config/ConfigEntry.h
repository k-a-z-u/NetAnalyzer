/*
 * ConfigEntry.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef CONFIGENTRY_H_
#define CONFIGENTRY_H_

#include <vector>
#include <string>

#include "ConfigValue.h"


// forward declaration
namespace tinyxml2 {
	class XMLElement;
}

/**
 * represents one entry within the XML-tree
 */
class ConfigEntry {

public:

	/** dtor */
	virtual ~ConfigEntry();

	/** request entry by path (e.g. "sniffer.device") */
	const ConfigEntry get(const std::string& path, bool mandatory) const;

	/** get all child elements with the given tag name (e.g. "compressor") */
	const std::vector<ConfigEntry> getAll(const std::string& name) const;

	/** get the value of this entry (if any) */
	const ConfigValue getValue() const;

	/** get attribute by it's name */
	const ConfigValue getAttribute(const std::string& name, bool mandatory) const;

	/** is this a valid ConfigEntry or a dummy */
	bool isValid() const;

protected:

	friend class Config;

	/** ctor */
	ConfigEntry(const tinyxml2::XMLElement* node);

private:

	/** the xml-node this entry represents */
	const tinyxml2::XMLElement* root;

};

#endif /* CONFIGENTRY_H_ */
