/*
 * HuffmanTree.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#include "HuffmanTree.h"
#include "../../Debug.h"

#include "HuffmanTreeNode.h"
#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"


HuffmanTree::HuffmanTree(HuffmanTreeNode* root) : root(root) {

	// create Look-Up-Tables
	HuffmanEncodeLUT lut;
	fillLUT(lutEnc, root, 0, 0);

}

HuffmanTree::~HuffmanTree() {
	if (root != nullptr) {delete root; root = nullptr;}
}



HuffmanEncoder* HuffmanTree::getEncoder() {
	return new HuffmanEncoder(lutEnc);
}

HuffmanDecoder* HuffmanTree::getDecoder() {
	return new HuffmanDecoder(this);
}

/** get the root-node of this tree */
const HuffmanTreeNode* HuffmanTree::getRoot() const {
	return root;
}

void HuffmanTree::fillLUT(HuffmanEncodeLUT& lutEnc, const HuffmanTreeNode* node, uint32_t level, uint32_t bitCode) {

	// proceed with the left branch
	if (node->left != nullptr) {
		fillLUT(lutEnc, node->left, level+1, (bitCode << 1) | 0);
	}

	// process all nodes
	if (node->isLeaf) {

		// store the node's huffman-code in the look-up-table
		//lut.byte[node->uncompressed].isValid = true;
		lutEnc.byte[node->uncompressed].bitCode = bitCode;
		lutEnc.byte[node->uncompressed].numBits = level;

		// fill decode-lut
		//lutDec.bits[bitCode] = node->uncompressed;

	}

	// proceed with the right branch
	if (node->right != nullptr) {
		fillLUT(lutEnc, node->right, level+1, (bitCode << 1) | 1);
	}

}

