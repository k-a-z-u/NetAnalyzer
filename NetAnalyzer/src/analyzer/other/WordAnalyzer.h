/*
 * DictAnalyzer.h
 *
 *  Created on: May 6, 2013
 *      Author: kazu
 */

#ifndef WORDANALYZER_H_
#define WORDANALYZER_H_

#include <string>
#include <vector>
#include <map>


struct Word {

	std::string word;

	uint32_t cnt;

	Word(const std::string& word, uint32_t cnt) : word(word), cnt(cnt) {;}

};

/** stats of one word */
struct WordStats {

	/* number of occurences of this word */
	uint32_t cnt;

	/* the timestamp this word was last modified */
	uint64_t ts;

	/** ctor */
	WordStats() : cnt(0), ts(0) {;}

};

/**
 * simple scan for most common words within payload
 */
class WordAnalyzer {

public:

	/** ctor */
	WordAnalyzer();

	/** dtor */
	virtual ~WordAnalyzer();

	/** append the given data and analyze it for common words */
	void append(const uint8_t* data, uint32_t length);

	/** get current (sorted) stats into the provided vector */
	void getStats(std::vector<Word>& dst) const;

	/** get the number of words currently listed */
	uint32_t getNumWords() const;

private:

	/** add a new word */
	void addWord(const std::string& str, uint64_t ts);

	/** remove too-old words from the list */
	void purge();

	/** map for all words */
	std::map<std::string, WordStats> words;

	/** timestamp of the last action */
	uint64_t lastInsertMs;

	/** disable the analyzer */
	bool disabled;

};

#endif /* WORDANALYZER_H_ */
