/*
 * ConfigEntry.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#include "ConfigEntry.h"
#include "../lib/tinyxml2/tinyxml2.h"
#include "Config.h"


ConfigEntry::ConfigEntry(const tinyxml2::XMLElement* node) : root(node) {
	;
}

ConfigEntry::~ConfigEntry() {
	// TODO Auto-generated destructor stub
}


const ConfigEntry ConfigEntry::get(const std::string& path, bool mandatory) const {

	const tinyxml2::XMLElement* node = this->root;
	uint32_t start = 0;
	while(true) {

		// get next element
		size_t pos = path.find('.', start);
		std::string layer = path.substr(start, pos-start);

		// get next layer from XML
		node = node->FirstChildElement(layer.c_str());

		// not found?
		if (node == nullptr) {break;}

		// done?
		if (pos == std::string::npos) {return ConfigEntry(node);}

		// next
		start = pos + 1;

	}

	// not found!
	if (!mandatory) {return ConfigEntry(nullptr);}
	Config::onError("mandatory entry '" + path + "' requested but not found!");
	return nullptr;

}

const std::vector<ConfigEntry> ConfigEntry::getAll(const std::string& name) const {

	// the found child-elements
	std::vector<ConfigEntry> entries;

	// if this entry is not valid (not found within tree) return an empty list
	if (!isValid()) {return entries;}

	// get the first element with tag-name "name"
	const tinyxml2::XMLElement* node = root->FirstChildElement(name.c_str());

	// proceed only if found
	if (node != nullptr) {

		// add the first node
		entries.push_back(ConfigEntry(node));

		while (true) {

			// add all following nodes
			node = node->NextSiblingElement(name.c_str());

			// done?
			if (node == nullptr) {break;}

			// add and proceed
			entries.push_back(ConfigEntry(node));

		}

	}

	// done
	return entries;

}

const ConfigValue ConfigEntry::getValue() const {
	if (root == nullptr) {return ConfigValue(nullptr);}
	if (root->Value() == nullptr) {return ConfigValue(nullptr);}
	return ConfigValue(root->GetText());
}

/** get attribute by it's name */
const ConfigValue ConfigEntry::getAttribute(const std::string& name, bool mandatory) const {

	// check if this element is valid
	if (!isValid()) {
		if (!mandatory) {return ConfigValue(nullptr);}
		Config::onError("mandatory attribute '" + name + "' requested but parent ConfigEntry not found!");
	}

	// get the attribute
	const char* ret = root->Attribute(name.c_str());
	if (ret == nullptr) {
		if (!mandatory) {return ConfigValue(nullptr);}
		Config::onError("node '" + std::string(root->Name()) + "' is missing the mandatory attribute '" + name + "'!");
	}

	// OK
	return ConfigValue(ret);

}

bool ConfigEntry::isValid() const {
	return root != nullptr;
}
