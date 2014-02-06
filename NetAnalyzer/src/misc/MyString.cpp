/*
 * MyString.cpp
 *
 *  Created on: 03.08.2012
 *      Author: kazu
 */

#include "MyString.h"
#include "../Debug.h"

/** trim a string */
std::string strTrim(std::string& str) {
	std::string::size_type pos = str.find_last_not_of(' ');
	if(pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != std::string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
	return str;
}

/** convert string to int */
int strToInt(const std::string& str) {
	return atoi( str.c_str() );
}

/** convert int to string */
std::string strFromInt(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

/** convert string to int */
float strToFloat(const std::string& str) {
	return atof( str.c_str() );
}

/** convert float to string */
std::string strFromFloat(float f) {
	std::stringstream ss;
	ss << f;
	return ss.str();
}

/** check if string starts with something */
bool strStartsWith(const std::string& str, const std::string& other) {
	return str.compare(0, other.length(), other) == 0;
}

/** check if string ends with something */
bool strEndsWith(const std::string& str, const std::string& ending) {
	return str.compare(str.length() - ending.length(), ending.length(), ending) == 0;
}


/** remove whitespaces */
void strRemoveSpaces(std::string& str) {
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

/** remove all occurences of char */
void strRemoveChar(std::string& str, char c) {
	str.erase (std::remove(str.begin(), str.end(), c), str.end());
}

/** remove all occurences of chars */
void strRemoveChars(std::string& str, const char* chars) {
	unsigned int len = strlen(chars);
	for (unsigned int i = 0; i < len; ++i) {
		str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

/** split a string using the provided char */
unsigned int strSplit(const std::string& str, std::vector<std::string>& strs, char ch) {

	size_t pos = str.find( ch );
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while( pos != std::string::npos ) {
		strs.push_back( str.substr( initialPos, pos - initialPos ) );
		initialPos = pos + 1;
		pos = str.find( ch, initialPos );
	}

	// Add the last one
	strs.push_back( str.substr( initialPos, std::min( pos, str.size() ) - initialPos + 1 ) );

	return strs.size();

}


/** replace all occurences of needle in haystack with replacement */
void strReplaceChar(std::string& haystack, char needle, char replacement) {
	replace(haystack.begin(), haystack.end(), needle, replacement);
}

/** replace all occurences of needle in haystack with replacement */
void strReplace(std::string& haystack, const std::string& needle, const std::string& replacement) {
	size_t startPos = 0;
	while((startPos = haystack.find(needle, startPos)) != std::string::npos) {
		haystack.replace(startPos, needle.length(), replacement);
		startPos += replacement.length();
	}
}

