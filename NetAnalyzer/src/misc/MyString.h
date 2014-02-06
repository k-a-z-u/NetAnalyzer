/*
 * MyString.h
 *
 *  Created on: 03.08.2012
 *      Author: kazu
 */

#ifndef MYSTRING_H_
#define MYSTRING_H_

#include <string.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

/** trim a string */
std::string strTrim(std::string& str);

/** convert string to int */
int strToInt(const std::string& str);

/** convert int to string */
std::string strFromInt(int i);

/** convert string to float */
float strToFloat(const std::string& str);

/** convert float to string */
std::string strFromFloat(float f);


/** replace all occurences of needle in haystack with replacement */
void strReplaceChar(std::string& haystack, char needle, char replacement);

/** replace all occurences of needle in haystack with replacement */
void strReplace(std::string& haystack, const std::string& needle, const std::string& replacement);

/** check if string starts with something */
bool strStartsWith(const std::string& str, const std::string& other);

/** check if string ends with something */
bool strEndsWith(const std::string& str, const std::string& other);

/** remove all whitespaces */
void strRemoveSpaces(std::string& str);

/** remove all occurences of char */
void strRemoveChar(std::string& str, char c);

/** remove all occurences of chars */
void strRemoveChars(std::string& str, const char chars[]);

/** split a string using the provided char */
unsigned int strSplit(const std::string& str, std::vector<std::string>& strs, char ch);

#endif /* MYSTRING_H_ */
