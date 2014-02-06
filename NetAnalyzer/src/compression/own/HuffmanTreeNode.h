/*
 * HuffmanTreeLeaf.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef HUFFMANTREELEAF_H_
#define HUFFMANTREELEAF_H_

#include <cstddef>


/** value 256 == end of huffman stream */
#define HUFFMAN_EOF		256


struct HuffmanCompressed {

	/** the bit-combination for this entry */
	uint32_t bits;

	/** the number of bits to use */
	uint8_t numBits;

};

/**
 * one node within the huffman tree.
 * it might be a leaf if has no left/right element following
 */
struct HuffmanTreeNode {

	/** if this is a leaf: the uncompressed byte or Command-byte (e.g. EOF) */
	uint32_t uncompressed;

	/** the "probability" of this leaf */
	uint32_t occurrence;

	/** is this a leaf? */
	bool isLeaf;


	/** left child, or null */
	HuffmanTreeNode* left;

	/** right child, or null */
	HuffmanTreeNode* right;

	/** ctor */
	HuffmanTreeNode() :
		uncompressed(0), occurrence(0), isLeaf(true), left(nullptr), right(nullptr) {;}

	/** ctor */
	HuffmanTreeNode(uint16_t uncompressed, uint32_t occurrence) :
		uncompressed(uncompressed), occurrence(occurrence), isLeaf(true), left(nullptr), right(nullptr) {;}

	/** dtor */
	~HuffmanTreeNode() {
		if (left != nullptr)	{delete left; left = nullptr;}
		if (right != nullptr)	{delete right; right = nullptr;}
	}

};



#endif /* HUFFMANTREELEAF_H_ */
