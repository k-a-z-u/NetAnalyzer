/*
 * Huffman.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#include "HuffmanBuilder.h"

#include <cstring>
#include <iostream>
#include <set>

#include "HuffmanTree.h"
#include "HuffmanTreeNode.h"
#include "HuffmanEncodeLUT.h"
#include "HuffmanEncoder.h"

#include "../../Debug.h"
#include "../../misc/MyString.h"


HuffmanBuilder::HuffmanBuilder(bool ensureAllBytes) : cntTotal(257) {

	// initialize all values with 1 ->
	// every character will be part of the huffman-tree
	// even those that were not appended()
	uint8_t val = (ensureAllBytes) ? (1) : (0);
	for (uint32_t i = 0; i <= 0xFF; ++i) {counter[i] = val;}

	// the end-of-stream marker
	counter[256] = 1;

}

HuffmanBuilder::~HuffmanBuilder() {
	// TODO Auto-generated destructor stub
}

const uint32_t* HuffmanBuilder::getCounterValues() const {
	return counter;
}

uint64_t HuffmanBuilder::getTotalValues() const {
	return cntTotal;
}

void HuffmanBuilder::setCounterValues(const std::vector<uint32_t>& values) {
	assertEQ(256, values.size(), "one must provide 256 counter values for setCounterValues(), " << values.size() << " given!");
	cntTotal = 0;
	for (uint32_t i = 0; i <= 0xFF; ++i) {
		counter[i] = values[i];
		cntTotal += values[i];
	}
}

void HuffmanBuilder::append(const uint8_t* data, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		++counter[data[i]];
	}
	cntTotal += length;
}

void HuffmanBuilder::dump() {
	for (uint32_t i = 0; i <= 0xFF; ++i) {
		std::cout.width(3);
		std::cout << i << " (";
		std::cout << ((i > 31 && i < 127) ? ((char)i) : (' ')) << "): ";
		std::cout.width(7);
		std::cout << counter[i] << " | ";
		if (i % 8 == 7) {std::cout << std::endl;}
	}
	std::cout << "------------------------------------" << std::endl;
}


float HuffmanBuilder::getEntropy() const {

	double sum = 0;

	for (uint32_t i = 0; i <= 0xFF; ++i) {
		double probability = counter[i] / (double) cntTotal;
		sum += probability * log2(1 / probability);
	}

	return sum;

}


uint64_t HuffmanBuilder::getCompressedSize() const {

	// TODO better solutions here?

	// first we have to construct the whole tree
	HuffmanTree* tree = getTree();

	// we need to get an encoder that creates us a LUT */
	HuffmanEncoder* enc = tree->getEncoder();

	// we use the LUT to guess compression rate
	const HuffmanEncodeLUT& lut = enc->getLUT();

	uint64_t uncompressedBits = 0;
	uint64_t compressedBits = 0;

	// "compress" each character once and sum it up
	for (uint32_t i = 0; i <= 0xFF; ++i) {
		compressedBits += lut.byte[i].numBits * counter[i];
		uncompressedBits += counter[i] * 8;
	}

	// cleanup
	delete enc;
	delete tree;

	return ceil(compressedBits / 8.0);

}

/** convert the gathered data into a huffman tree */
HuffmanTree* HuffmanBuilder::getTree() const {

	// create a set that automatically sorts all entries by their probability
	auto comparator = [](const HuffmanTreeNode* a, const HuffmanTreeNode* b) -> bool { return a->occurrence > b->occurrence; };
	std::multiset <HuffmanTreeNode*, decltype(comparator) > huffSet(comparator);

	// add all entries to the SORTED set. (high occurrences first)
	for (uint32_t i = 0; i <= 0xFF+1; ++i) {

		// ignore entries with no occurences
		if (counter[i] == 0) {continue;}

		// create a new node and insert it into the sorted set
		HuffmanTreeNode* node = new HuffmanTreeNode(i, counter[i]);
		node->isLeaf = true;
		huffSet.insert(node);

	}

	// create the tree
	// 1) remove smallest two entries from the set
	// 2) combine them to a node (the smaller of the two is right)
	// 3) the probability of the node is the sum of it's two leafs
	// 4) add the node to the set
	// 5) proceed until only one node (root) in the set
	while (huffSet.size() > 1) {

		// get the two unlikeliest entries
		std::set<HuffmanTreeNode*>::iterator it = --huffSet.end();

		// be careful here!! must be "it--"
		// to decrement iterator correctly and delete the entry
		// else either of both will fail (valgrind: invalid read of ...)
		HuffmanTreeNode* n1 = *(it);
		huffSet.erase(it--);

		HuffmanTreeNode* n2 = *(it);
		huffSet.erase(it);

		// combine them to a new node
		HuffmanTreeNode* node = new HuffmanTreeNode();

		node->occurrence = n1->occurrence + n2->occurrence;
		node->isLeaf = false;

		if (n1->occurrence > n2->occurrence) {
			node->left = n1;
			node->right = n2;
		} else {
			node->left = n2;
			node->right = n1;
		}

		// mark as leaf?
		node->isLeaf = node->left == nullptr && node->right == nullptr;

		// append the new node to the set
		huffSet.insert(node);

	}

	return new HuffmanTree( *(huffSet.begin()) );

}

void HuffmanBuilder::loadFromString(const std::string& values, char sep) {

	std::vector<std::string> tree;
	std::vector<uint32_t> treeInt;

	// split by separator
	strSplit(values, tree, sep);

	// convert to uint32_t
	for (const std::string& val : tree) {
		treeInt.push_back(atoi(val.c_str()));
	}

	// apply values
	setCounterValues(treeInt);

}
