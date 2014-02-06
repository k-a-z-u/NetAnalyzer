/*
 * WordAnalyzer.cpp
 *
 *  Created on: May 6, 2013
 *      Author: kazu
 */

#include "WordAnalyzer.h"
#include <algorithm>
#include "../../Helper.h"

// remove words if they haven't occured for 30 seconds
#define DICT_PURGE_INTERVAL		1000 * 30



WordAnalyzer::WordAnalyzer() : lastInsertMs(0), disabled(true) {
	// TODO Auto-generated constructor stub

}

WordAnalyzer::~WordAnalyzer() {
	// TODO Auto-generated destructor stub
}

void WordAnalyzer::getStats(std::vector<Word>& vec) const {

	vec.clear();
	for (const auto& word : words) {
		vec.push_back(Word(word.first, word.second.cnt));
	}

	// sort the vector
	const auto lambda = [](const Word& a, const Word& b) {return a.cnt*a.word.length() > b.cnt*a.word.length();};
	std::sort(vec.begin(), vec.end(), lambda);

}

uint32_t WordAnalyzer::getNumWords() const {
	return words.size();
}

void WordAnalyzer::addWord(const std::string& str, uint64_t ts) {
	lastInsertMs = ts;
	words[str].cnt ++;
	words[str].ts = ts;

}

void WordAnalyzer::purge() {

	auto it = words.begin();
	while (it != words.end()) {
		if (lastInsertMs - it->second.ts > DICT_PURGE_INTERVAL) {
			auto toDelete = it;
			++it;
			words.erase(toDelete);
		} else {
			++it;
		}
	}

}

void WordAnalyzer::append(const uint8_t* data, uint32_t length) {

	if (disabled) {return;}

	uint64_t ts = getTimeStampMS();
	uint32_t start = 0;
	uint32_t end = 0;

	// split data into words
	for (uint32_t i = 0; i < length; ++i) {

		// get next char and check if it's a splitter
		char c = data[i];
		if ( ! ( (c >= 65 && c <= 90) || (c >= 97 && c <= 122) ) ) {

			// mark the word's end
			end = i;

			// check if the word is not-empty
			if (end - start >= 3) {
				addWord(std::string( (const char*) data+start, end-start), ts);
			}

			// next word starts at next char
			start = end + 1;

		}

	}

	purge();

}
