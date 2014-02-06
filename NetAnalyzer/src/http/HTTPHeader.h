/*
 * HTTPHeader.h
 *
 * a parsed HTTP header with key-value pairs
 *
 *  Created on: Apr 11, 2013
 *      Author: kazu
 */

#ifndef HTTPHEADER_H_
#define HTTPHEADER_H_

#define HTTP_METHOD_GET			1
#define HTTP_METHOD_POST		2
#define HTTP_METHOD_HEAD		3
#define HTTP_METHOD_RESPONSE	8


#include <map>
#include <string>
#include <cstdint>

class HTTPHeader {

public:

	/** ctor */
	HTTPHeader();

	/** dtor */
	virtual ~HTTPHeader();

	/** get the value of the given key */
	const std::string& get(const std::string& key) const;

	/** set a (new) key-value pair */
	void set(const std::string& key, const std::string& val);

	/** does the header contain the given key? */
	bool containsKey(const std::string& key) const;

	/** does the header contain the given key->value pair? */
	bool has(const std::string& key, const std::string& val) const;


	/** set the method of this header (GET, POST, RESPONSE, ...) */
	void setMethod(uint8_t method);

	/** get the method of this header (GET, POST, RESPONSE, ...) */
	uint8_t getMethod() const;

	/** get the requested url (if this was a request) */
	const std::string& getURL() const;

	/** set the response code */
	void setCode(uint16_t code);

	/** get the response code if this was a response */
	uint16_t getCode() const;

	/** get the header's length in bytes */
	uint16_t getLength() const;

	/** get the content-type of the payload if this was a response*/
	const std::string& getContentType() const;

	/** is this a request (or response) header */
	bool isRequestHeader() const;

	/** get the timestamp this header has been created */
	uint64_t getTimestamp() const;



	/** debug-dump this header */
	void dump() const;

	/** create header from the given header-string */
	bool loadFromString(const std::string& header);


private:

	/** hidden copy ctor */
	HTTPHeader(const HTTPHeader&);


	/** parse one single line of the header */
	void parseLine(const std::string& line);

	/** parse the first line of the header */
	bool parseFirstLine(const std::string& line);


	/** store all key->value pairs here */
	std::map<std::string, std::string> entries;

	/** the requested URL if this was a request, else: empty */
	std::string url;

	/** the method behind the header. GET, POST, ... */
	uint8_t method;

	/** the response code, if this was a response-header */
	uint16_t code;

	/** the original length of the header in bytes */
	uint16_t length;

	/** the timestamp this header has been created */
	uint64_t timestamp;

};

#endif /* HTTPHEADER_H_ */
