/*
 * ConfigValue.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */


#include <sstream>

#include "ConfigValue.h"


ConfigValue::ConfigValue(const char* value) : value(value) {
	// TODO Auto-generated constructor stub

}

ConfigValue::~ConfigValue() {
	// TODO Auto-generated destructor stub
}

bool ConfigValue::isValid() const {
	return value != nullptr;
}

int ConfigValue::asInt(int def) const {
	if (!isValid()) {return def;}
	std::stringstream ss;
	int ret;
	ss << value;
	ss >> ret;
	return ret;
}

const std::string ConfigValue::asString(const std::string& def) const {
	if (!isValid()) {return def;}
	return value;
}

bool ConfigValue::asBool(const bool def) const {
	if (!isValid()) {return def;}
	return true;
}
