/*
 * HTTPHeader.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#include "HTTPHeader.h"
#include "../Debug.h"
#include "../Helper.h"

#include "convInt.h"

#include <algorithm>
#include <iostream>

#define DBG_HEADER		"HEADER"

#define HEADER_END		"\r\n\r\n"


HTTPHeader::HTTPHeader() : method(0), code(0), length(0) {
	timestamp = getTimeStampMS();
}

HTTPHeader::~HTTPHeader() {

}


bool HTTPHeader::loadFromString(const std::string& header) {

	// store the header's length
	this->length = header.length();

	// sanity checks
	if (header.find(HEADER_END, length-4) == std::string::npos) {
		debug(DBG_HEADER, DBG_LVL_ERR, "header does not end with \\r\\n\\r\\n!");
		return false;
	}

	uint32_t start = 0;
	uint32_t end = 0;

	while ( true ) {

		// get position of the next "\r\n"
		end = header.find("\r\n", start);

		// have we found the headers end? (immediately following "\r\n")
		if (start == end) {

			// header empty???
			if (start == 0) {
				debug(DBG_HEADER, DBG_LVL_ERR, "detected an empty header!");
				return false;
			}

			// ok! header seems valid
			break;

		}

		// get this line and parse it's content
		// handle the first line differently
		if (start == 0) {
			if (!parseFirstLine( header.substr(start, end-start) )) {
				return false;
			}
		} else {
			parseLine( header.substr(start, end-start) );
		}

		// proceed after the line-terminating "\r\n"
		start = end + 2;

	}

	// header was valid
	return true;

}

void HTTPHeader::dump() const {
	for (const auto& it : entries) {
		std::cout << it.first << " -> " << it.second << std::endl;
	}
}

const std::string& HTTPHeader::getURL() const {
	return url;
}

void HTTPHeader::parseLine(const std::string& line) {

	// get the position of the splitting ":"
	const size_t pos = line.find(':');
	if (pos == std::string::npos) {return;}

	// split into key->val
	// detect empty value parts and assign an empty string to them
	std::string key = line.substr(0, pos);
	std::string val = (pos+2 < line.length()) ? (line.substr(pos + 2)) : ("");

	// make lower-case for easy case-insensitivity
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	std::transform(val.begin(), val.end(), val.begin(), ::tolower);

	// debug assertions
	assertLowerCase(key, "header-key is not lower-case: " << key);
	assertLowerCase(key, "header-val is not lower-case: " << val);

	// if value contains multiple values (split by ;) add only the first one
	const size_t pos1 = val.find(';');
	if (pos1 != std::string::npos) {val = val.substr(0, pos1);}

	// store
	entries[key] = val;

}



bool HTTPHeader::parseFirstLine(const std::string& line) {

	if		(line.find("POST", 0, 4) == 0)	{
		method = HTTP_METHOD_POST;
		url = line.substr(5, line.find(' ', 5) - 5);

	} else if	(line.find("GET", 0, 3) == 0)	{
		method = HTTP_METHOD_GET;
		url = line.substr(4, line.find(' ', 4) - 4);

	} else if	(line.find("HEAD", 0, 4) == 0)	{
		method = HTTP_METHOD_HEAD;
		url = line.substr(5, line.find(' ', 5) - 5);

	} else if (line.find("HTTP", 0, 4) == 0)	{
		method = HTTP_METHOD_RESPONSE;
		uint32_t pos = line.find(' ');
		std::string code = line.substr(pos+1, 3);
		this->code = strToInt(code);

	} else {
		debug(DBG_HEADER, DBG_LVL_ERR, "bogus header detected. first line was: '" << line << "'");
		return false;

	}

	// everything fine
	return true;

}



const std::string& HTTPHeader::get(const std::string& key) const {
	assertLowerCase(key, "header.get() MUST use lower-case strings for 'key'! key was: " << key);
	return entries.find(key)->second;
}

void HTTPHeader::set(const std::string& key, const std::string& val) {
	entries[key] = val;
}

bool HTTPHeader::containsKey(const std::string& key) const {
	assertLowerCase(key, "header.containsKey() MUST use lower-case strings for 'key'! key was: " << key);
	return entries.find(key) != entries.end();
}

bool HTTPHeader::has(const std::string& key, const std::string& val) const {
	assertLowerCase(key, "header.has() MUST use lower-case strings for 'key'! key was: " << key);
	assertLowerCase(key, "header.has() MUST use lower-case strings for 'val'! val was: " << val);
	if (!containsKey(key)) {return false;}
	return get(key).compare(val) == 0;
}

void HTTPHeader::setMethod(uint8_t method) {
	this->method = method;
}

uint8_t HTTPHeader::getMethod() const {
	return method;
}

void HTTPHeader::setCode(uint16_t code) {
	this->code = code;
}

uint16_t HTTPHeader::getCode() const {
	return code;
}

uint16_t HTTPHeader::getLength() const {
	return length;
}

uint64_t HTTPHeader::getTimestamp() const {
	return timestamp;
}

const std::string& HTTPHeader::getContentType() const {
	static std::string UNKNOWN = "unknown";
	if (entries.find("content-type") == entries.end()) {return UNKNOWN;}
	return entries.find("content-type")->second;
}

bool HTTPHeader::isRequestHeader() const {
	// sanity check
	assertTrue(getMethod() != 0, "header seems invalid! method is not set!");
	return getMethod() != HTTP_METHOD_RESPONSE;
}



