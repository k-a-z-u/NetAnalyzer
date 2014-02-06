/*
 * ConfigValue.h
 *
 *  Created on: Apr 23, 2013
 *      Author: kazu
 */

#ifndef CONFIGVALUE_H_
#define CONFIGVALUE_H_


/**
 * represents a value (attribute or text-value) within the XML-tree
 * that can be converted to several types
 */
class ConfigValue {
public:

	virtual ~ConfigValue();

	/** is this a valid value? */
	bool isValid() const;

	/** convert value to integer. use def if this value is not valid */
	int asInt(int def) const;

	/** convert value to string. use def if this value is not valid */
	const std::string asString(const std::string& def) const;

	/** convert value to boolean. use def if this value is not valid */
	bool asBool(const bool def) const;

protected:

	friend class ConfigEntry;

	/** ctor */
	explicit ConfigValue(const char *);

private:

	/** the value that can be formatted */
	const char* value;

};

#endif /* CONFIGVALUE_H_ */
