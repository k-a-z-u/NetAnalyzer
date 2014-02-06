/*
 * TestHeader.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: kazu
 */

#ifdef WITH_TESTS

#include "Test.h"
#include "../http/HTTPHeader.h"

/** empty header */
TEST(HttpHeader, Empty) {
	std::string header = "\r\n\r\n";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_FALSE(ok);
}

/** single line header */
TEST(HttpHeader, OneLine) {

	std::string header;
	bool ok;

	// first try
	header = "GET /index.html HTTP1.0\r\n\r\n";
	HTTPHeader h1;
	ok = h1.loadFromString(header);
	ASSERT_TRUE(ok);
	ASSERT_EQ(HTTP_METHOD_GET, h1.getMethod());
	ASSERT_STREQ("/index.html", h1.getURL().c_str());

	// second try
	header = "HTTP/1.1 200 OK\r\n\r\n";
	HTTPHeader h2;
	ok = h2.loadFromString(header);
	ASSERT_TRUE(ok);
	ASSERT_EQ(HTTP_METHOD_RESPONSE, h2.getMethod());
	ASSERT_STREQ("", h2.getURL().c_str());

}

/** test empty value field within header */
TEST(HttpHeader, EmptyValue1) {

	std::string header = "GET / HTTP1.0\r\nkey1: val1\r\nkey2:\r\n\r\n";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_TRUE(ok);
	ASSERT_STREQ("val1",	h.get("key1").c_str());
	ASSERT_STREQ("",		h.get("key2").c_str());

}

/** test empty value (including whitespace) field within header */
TEST(HttpHeader, EmptyValue2) {

	std::string header = "GET / HTTP1.0\r\nkey1: val1\r\nkey2: \r\n\r\n";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_TRUE(ok);
	ASSERT_STREQ("val1",	h.get("key1").c_str());
	ASSERT_STREQ("",		h.get("key2").c_str());

}

/** test empty value (including two whitespaces) field within header */
TEST(HttpHeader, EmptyValue3) {

	std::string header = "GET / HTTP1.0\r\nkey1: val1\r\nkey2:  \r\n\r\n";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_TRUE(ok);
	ASSERT_STREQ("val1",	h.get("key1").c_str());
	ASSERT_STREQ(" ",		h.get("key2").c_str());

}

/** test for a bogus header start */
TEST(HttpHeader, BogusStart) {

	std::string header = " GET / HTTP1.0\r\nkey1: val1\r\nkey2:\r\n\r\n";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_FALSE(ok);

}

/** test for missing header end */
TEST(HttpHeader, MissingEnd) {

	std::string header = "GET / HTTP1.0\r\nkey1: val1\r\nkey2:\r\n\r";
	HTTPHeader h;
	bool ok = h.loadFromString(header);
	ASSERT_FALSE(ok);

}

#endif
