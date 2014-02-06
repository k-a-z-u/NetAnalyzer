/*
 * LZWDict.h
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */

#ifndef LZWDICT_H_
#define LZWDICT_H_

//#include <unordered_map>
#include <cstdint>
#include <string>

#include "LZWWord.h"
#include "opt.h"

//#include "LZWHashSet.h"
#include "LZWTreeNode.h"

/** see CTOR */
#define LZW_DICT_EOF				256

#define	LZW_DICT_SIZE				(1<<16)
#define LZW_DICT_MAX_IDX			(LZW_DICT_SIZE-1)


#define DBG_LZW_DICT				"LZWDict"
#include "../../Debug.h"

#include <iostream>
#include "../../Helper.h"
/**
 * the dictionary behind LZW-based compression.
 * (currently uses c++ containers)
 */
class LZWDict {
public:

	/**
	 * the dictionary starts with 256 entries (1 for each byte)
	 * followed by the EOF marker ->
	 * the first word gets the index of EOF-marker + 1
	 */
	inline LZWDict() : nextIdx(LZW_DICT_EOF+1), longestWord(0) {
		init();
	}

	/** dtor */
	inline ~LZWDict() {
		;
	}

//	/**
//	 * get the index of this word within the dictionary.
//	 * returns LZW_DICT_NOT_FOUND if not contained in dictionary
//	 */
//	inline uint32_t getIdx(const LZWWord& word) const {
//		return enc.get(word);
//	}

	/**
	 * get the word behind the given index
	 */
	inline const LZWDecWord& getWord(uint32_t idx) const {
		return dec[idx];
	}


	/**
	 * get the dictionary tree used for fast encoding
	 */
	inline const LZWTreeNode* getEncTree() const {
		return &encTree;
	}

	/**
	 * append the given data to the dictionary.
	 * used for building a dictionary from a given "string"
	 */
	inline void append(const uint8_t* data, uint32_t length) {

		static LZWTreeNode* curNode = &encTree;
		static LZWDecWord pending;


		// append data
		while (length--) {

			// check whether the dictionary is full?
			if (nextIdx > LZW_DICT_MAX_IDX) {
				debug(DBG_LZW_DICT, DBG_LVL_WARN, "dict is now full!");
				return;
			}

			// dictionary entries's size MUST NOT be longer than LZW_WORD_LEN-1 bytes!
			if (pending.length >= LZW_WORD_LEN) {pending.clear(); curNode = &encTree;}

			// TODO (seems to provide a slight optimization of 1%)
			//if (*data == '<') {pending.clear(); curNode = &encTree;}


			// TODO better way?
			// needed for decoder...
			pending.add(*data);

			// if this word is new to the dict -> add it, and start over
			if (!curNode->nodes[*data]) {

				// create a new node for this entry
				LZWTreeNode* newNode = new LZWTreeNode();
				newNode->idx = nextIdx;
				curNode->nodes[*data] = newNode;

				//std::cout << " -> miss! (" << nextIdx << ")" << std::endl;

				// add decode-mapping
				dec[nextIdx] = pending;

				// start over
				++nextIdx;
				curNode = &encTree;
				pending.clear();

				// better compression??
				pending.add(*data);
				curNode = curNode->nodes[*data];

			} else {

				// traverse the tree
				curNode = curNode->nodes[*data];

			}

			// next byte
			++data;




// OLD
//			// dictionary entries's size MUST NOT be longer than LZW_WORD_LEN-1 bytes!
//			if (pending.length >= LZW_WORD_LEN) {pending.clear();}
//
//			// append "byte" to pending word
//			pending.add(*data);

//			// if this word is new to the dict -> add it, and start over
//			if ( enc.get(pending) == LZW_DICT_NOT_FOUND ) {
//
//				//std::cout << "idx:" << nextIdx << " word:" << std::string( (const char*)pending.data.bytes, pending.length) << std::endl;
//
//				// add encode-mapping
//				enc.add(pending, nextIdx);
//
//				// add decode-mapping
//				dec[nextIdx] = pending;
//
//				// start over
//				++nextIdx;
//				pending.clear();
//
//				// TODO: compression seems a bit better with this line
//				//pending.add(*data);
//
//			}
//
//			// next byte
//			++data;

		}

	}

	/**
	 * see: append(data, length)
	 */
	inline void append(const std::string& str) {
		append( (uint8_t*) str.c_str(), str.length());
	}

	inline void loadFile(const std::string& fileName) {
		FILE* handle = fopen(fileName.c_str(), "r");
		if (!handle) {error(DBG_LZW_DICT, "could not read requested file: " << fileName);}
		uint8_t buffer[4096];
		while(true) {
			uint32_t numRead =  fread(buffer, 1, sizeof(buffer), handle);
			append(buffer, numRead);
			if (numRead != sizeof(buffer)) {return;}
		}
		fclose(handle);
	}

private:

	/** hidden */
	LZWDict(const LZWDict&);
	LZWDict& operator=(const LZWDict&);

	/** initialize dictionary with default characters */
	void init() {

		LZWDecWord word;

		// init the first 256 bytes
		for (uint32_t i = 0; i <= 0xFF; ++i) {

			// for encoding
			LZWTreeNode* newNode = new LZWTreeNode();
			newNode->idx = i;
			encTree.nodes[i] = newNode;

			// for decoding
			word.clear();
			word.add(i);
			dec[i] = word;

		}


//		LZWWord word;
//
//		// add all single bytes to the dict to ensure every input is compressible
//		for (uint32_t i = 0; i <= 0xFF; ++i) {
//			word.clear();
//			word.add(i);
//			enc.add(word, i);
//			dec[i] = word;
//		}



	}




	/** encoding: word -> int mapping using a hash-map */
	//std::unordered_map<LZWWord, uint32_t, LZWWordHash, LZWWordEqual> enc;

	/** encoding: word(hash) -> int */
	//LZWWord enc[LZW_DICT_SIZE];
	//LZWHashSet enc;

	/** tree for encoding (hopefully faster than the hash-set above */
	LZWTreeNode encTree;

	/** decoding: int -> word */
	LZWDecWord dec[LZW_DICT_SIZE];



	/** the integer to assign to the next word */
	uint32_t nextIdx;

	/** for stats only */
	uint32_t longestWord;



};

#endif /* LZWDICT_H_ */
