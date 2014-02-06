/*
 * LZWDictEncoder.cpp
 *
 *  Created on: May 22, 2013
 *      Author: kazu
 */


#include "LZWDictEncoder.h"
#include "LZWDictStream.h"
#include "LZWTreeNode.h"
#include "LZWDict.h"
#include "opt.h"

#include <string>

LZWDictEncoder::LZWDictEncoder(const LZWDict& dict) : dict(dict) {

	rootNode = dict.getEncTree();
	curNode = rootNode;

}

LZWDictEncoder::~LZWDictEncoder() {
	// TODO Auto-generated destructor stub
}

#include <iostream>
void LZWDictEncoder::append(const uint8_t* data, const uint32_t length, LZWDictStream& dst) {

	// process all bytes
	for (register uint32_t i = 0; i < length; ++i) {

		// check whether the current node + 1 char is within the dictionary
		if ( !curNode->nodes[data[i]] ) {

			// not found in dictionary -> use last index (the current node)
			// append to output stream
			dst.append(curNode->idx);

			// start over at the root
			curNode = rootNode;

		}

		// proceed with the next char
		curNode = curNode->nodes[data[i]];

	}



// OLD
//	/** store last matching index for faster decoding */
//	static uint32_t lastMatchIdx = 0;
//
//	for (register uint32_t i = 0; i < length; ++i) {
//
//		// append to pending
//		pending.add(data[i]);
//		//pending += (char) *data;
//
//		// get index from dictionary
//		uint32_t idx = dict.getIdx( pending );
//
//		// dict miss ?
//		if ( unlikely(idx == LZW_DICT_NOT_FOUND) ) {
//
//			// use the index of the word (without the current char)
//			// that matched the dictionary
//			dst.append(lastMatchIdx);
//
//			// then: proceed with current char
//			pending.clear();
//			pending.add(data[i]);
//			//pending.resetTo(data[i]);
//
//			// set the last-matching index to that one of this single char
//			// which is actually the value of the char itself
//			lastMatchIdx = data[i];
//
//		} else {
//
//			// store last matching index
//			lastMatchIdx = idx;
//
//		}
//
//
//
//	}
//

}

void LZWDictEncoder::close(LZWDictStream& dst) {

	// pending entry? -> flush
	if (curNode != rootNode) {
		dst.append(curNode->idx);
		curNode = rootNode;
	}

// OLD
//	// if we have pending data -> flush it
//	if (!pending.empty()) {
//		uint32_t idx = dict.getIdx(pending);
//		dst.append(idx);
//	}

	// append EOF
	//dst.append(LZW_DICT_EOF);

}
