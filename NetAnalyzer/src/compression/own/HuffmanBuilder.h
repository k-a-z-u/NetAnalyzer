/*
 * Huffman.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef HUFFMANBUILDER_H_
#define HUFFMANBUILDER_H_

#include <cstdint>
#include <vector>
#include <string>

// forward declaration
class HuffmanTree;



class HuffmanBuilder {

public:

	/**
	 * ctor
	 * EnsureAllBytes will init the counter with 1 for every value!
	 * this ensures that each byte (0-255) will be within the tree
	 */
	HuffmanBuilder(bool ensureAllBytes);

	/** dtor */
	virtual ~HuffmanBuilder();

	/** append the provided data to the huffman-probabilities */
	void append(const uint8_t* data, uint32_t length);

	/** dump the tree */
	void dump();

	/** convert the gathered data into a huffman tree */
	HuffmanTree* getTree() const;

	/**
	 * get the compressed size when applying huffman with the calculated tree
	 * to the given payload.
	 * this value might be off by a few bytes as it depends on the underlying
	 * bit-stream and its alignment size.
	 */
	uint64_t getCompressedSize() const;

	/** get the current 256 counting values for each byte */
	const uint32_t* getCounterValues() const;

	/** get the total number of tracked bytes */
	uint64_t getTotalValues() const;

	/** set new counter values for the huffman-builder */
	void setCounterValues(const std::vector<uint32_t>& values);

	/** load values from the given string. values are separated by 'sep' */
	void loadFromString(const std::string& values, char sep);

	/** get the entropy value for the analyzed payload */
	float getEntropy() const;

private:

	/** hidden copy ctor */
	//HuffmanBuilder(const HuffmanBuilder&);

	/** count the occurrences of all "letters" (+1 for EOF) */
	uint32_t counter[256+1];

	/** count number of "letters" */
	uint64_t cntTotal;

};

#endif /* HUFFMANBUILDER_H_ */
