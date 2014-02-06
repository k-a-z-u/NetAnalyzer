/*
 * LZWHashSet.h
 *
 *  Created on: May 23, 2013
 *      Author: kazu
 */

#ifndef LZWHASHSET_H_
#define LZWHASHSET_H_

#define LZW_HASH_SIZE			(1 << 20)
//#define LZW_DICT_HASH(word)		(word.getHash() & (LZW_HASH_SIZE-1))
#define LZW_DICT_HASH(word)		(word.getHash() % 1048573)

#define LZW_DICT_NOT_FOUND		0xFFFFFFFF

#include <cstdint>
#include "LZWWord.h"

#include <iostream>

#include <unistd.h>

/**
 * represents a HashSet used for storing LZWWords
 */
class LZWHashSet {

public:

	/** ctor */
	inline LZWHashSet() : buckets(nullptr) {

		// allocate space for all hash-buckets and initialize them with 0
		buckets = (LZWWord**) malloc(LZW_HASH_SIZE * sizeof(LZWWord*));
		memset(buckets, 0, LZW_HASH_SIZE * sizeof(LZWWord*));

	}

	/** hidden copy ctor */
	inline LZWHashSet(const LZWHashSet& ref) {
		// allocate space for all hash-buckets and initialize them with 0
		buckets = (LZWWord**) malloc(LZW_HASH_SIZE * sizeof(LZWWord*));
		memcpy(buckets, ref.buckets, LZW_HASH_SIZE * sizeof(LZWWord*));
	}

	/** dtor */
	inline ~LZWHashSet() {
		if (buckets) {free(buckets); buckets = nullptr;}
	}

	/** check whether the map contains the given word */
	inline uint32_t get(const LZWWord& word) const {

		// get hash of this word
		uint32_t hash = LZW_DICT_HASH(word);

		// bucket completely empty -> not in hash-map
		if (!buckets[hash]) {return LZW_DICT_NOT_FOUND;}

		// get matching result from linked list
		LZWWord* cur = buckets[hash];
		do {
			if (cur->equals(word)) {return cur->idx;}
			cur = cur->next;
		} while (cur);

		// no word in the linked list matched
		return LZW_DICT_NOT_FOUND;


	}



	/** add word to hash map */
	inline void add(const LZWWord& word, uint32_t idx) {

		static uint32_t collisions = 0;

		uint32_t hash = LZW_DICT_HASH(word);

		if (!buckets[hash]) {

			// free bucket -> attach
			buckets[hash] = new LZWWord(word);
			buckets[hash]->idx = idx;

		} else {

			// shared bucket
			++collisions;
			//std::cout << "num collisions:" << collisions << std::endl;
			LZWWord* cur = buckets[hash];

			// find next free slot in linked-list
			while (cur->next) {cur = cur->next;}

			// attach
			cur->next = new LZWWord(word);
			cur->next->idx = idx;

		}

	}

private:

	/** hidden */
	LZWHashSet& operator=(const LZWHashSet&);

	/** hash map bucket */
	LZWWord** buckets;

};

#endif /* LZWHASHSET_H_ */
