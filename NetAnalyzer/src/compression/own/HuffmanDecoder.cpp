/*
 * HuffmanDecoder.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: kazu
 */

#include "HuffmanDecoder.h"
#include "BitstreamIn.h"
#include "HuffmanTreeNode.h"
#include "HuffmanTree.h"
#include "opt.h"

/*
HuffmanDecoder::HuffmanDecoder(HuffmanTree* tree) : tree(tree) {

	node = tree->getRoot();

}
 */

HuffmanDecoder::HuffmanDecoder(const HuffmanTree* tree) {

	root = tree->getRoot();
	lastNode = root;

}

HuffmanDecoder::~HuffmanDecoder() {
	// TODO Auto-generated destructor stub
}




bool HuffmanDecoder::append(BitstreamIn& stream, std::vector<uint8_t>& dst) {

	// check available bytes
	uint32_t numWords = stream.getNumReadableWords();

	// process all words
	while(numWords--) {

		// get the next word from the bitstream
		register const BSI_TYPE value = stream.getWord();

		// process all bits
		for (uint32_t i = 0; i < BSI_TYPE_BITS; ++i) {

			// get the next bit
			const bool bit = ((value<<i) & BSI_BIT_MASK) != 0;

			// traverse the tree (ugly hack that works without branching)
			this->lastNode = *(&this->lastNode->left + bit);

			// sanity check
			// assertNotNull(node, "found a nullptr node while traversing Huffman-Tree!");
			if ( unlikely(this->lastNode == nullptr) ) {return false;}

			// if this is a leaf, append the uncompressed data
			// and re-start at the root
			if ( unlikely(this->lastNode->isLeaf) ) {

				// if this is the EOF marker -> done
				if ( unlikely(this->lastNode->uncompressed == HUFFMAN_EOF) ) {
					stream.reset();
					return true;
				}

				// append uncompressed byte to buffer
				dst.push_back(this->lastNode->uncompressed);

				// restart at the root-element
				this->lastNode = (HuffmanTreeNode*) root;

			}

		}

	}

	// everything fine
	return true;

}

/*
bool HuffmanDecoder::append(BitstreamIn& stream, std::vector<uint8_t>& dst) {

	stream.prefetch();

	// check available bits
	uint32_t numBits = stream.getNumReadableBits();

	// map to a local variable (seems much faster in benchmarks!)
	HuffmanTreeNode* curNode = (HuffmanTreeNode*) this->lastNode;

	// process all bits
	while(numBits--) {

		// get next bit from stream
		const bool bit = stream.getBit();

		// traverse the tree
		curNode = (bit) ? (curNode->right) : (curNode->left);
		//const size_t mask = -bit;
		//curNode = (HuffmanTreeNode*) ( (mask & (size_t)curNode->right) | (~mask & (size_t)curNode->left) );


		// sanity check
		// assertNotNull(node, "found a nullptr node while traversing Huffman-Tree!");
		if (curNode == nullptr) {return false;}


		if (!curNode->leaf) {continue;}

		// leaf? append uncompressed data and re-start at the root

		// if this is the EOF marker -> done
		if (curNode->uncompressed == HUFFMAN_EOF) {
			stream.reset();
			return true;
		}

		// append uncompressed byte to buffer
		dst.push_back(curNode->uncompressed);

		// restart at the root-element
		curNode = (HuffmanTreeNode*) root;

	}

	// map current state back to class
	this->lastNode = curNode;

	// everything fine
	return true;

}
*/
