/*
 * HuffmanTree.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef HUFFMANTREE_H_
#define HUFFMANTREE_H_

#include <cstdint>

#include "HuffmanEncodeLUT.h"
//#include "HuffmanDecodeLUT.h"

// forward declarations
struct HuffmanTreeNode;
class HuffmanEncoder;
class HuffmanDecoder;


class HuffmanTree {

public:

	/** ctor */
	HuffmanTree(HuffmanTreeNode* root);

	/** dtor */
	virtual ~HuffmanTree();

	/** get a new encoder for this tree */
	HuffmanEncoder* getEncoder();

	/** get a new decoder for this tree */
	HuffmanDecoder* getDecoder();

	/** get the root-node of this tree */
	const HuffmanTreeNode* getRoot() const;


private:

	/** traverse the tree and fill the LUT (reference) with values */
	void fillLUT(HuffmanEncodeLUT& lutEnc, const HuffmanTreeNode* node, uint32_t level, uint32_t bitCode);

	/** the root node */
	HuffmanTreeNode* root;

	HuffmanEncodeLUT lutEnc;

	//HuffmanDecodeLUT lutDec;

};

#endif /* HUFFMANTREE_H_ */
