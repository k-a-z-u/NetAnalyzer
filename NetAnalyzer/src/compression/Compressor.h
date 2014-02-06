/*
 * Compressor.h
 *
 *  Created on: Apr 17, 2013
 *      Author: kazu
 */

#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include <string>
#include <vector>
#include <cstddef>

struct Data;

/**
 * interface for all compressors
 */
class Compressor {

public:

	/** dtor */
	virtual ~Compressor() {;}

	/**
	 * Create a new Compressor object with the same settings as the original.
	 * This is not called 'clone()' because it is not intended to clone the
	 * whole object's internal status (buffer, etc.) but to preserve the
	 * original object's compression settings.
	 */
	virtual Compressor* create() const = 0;

	/** get the display-name of this compressor */
	std::string getName() const {return name;}

	/** set the display-name of this compressor */
	void setName(const std::string& name) {this->name = name;}


	/** open the compressor */
	virtual bool open() = 0;

	/**
	 * Find out how much memory the provided data would need when compressed.
	 * Use this append() function only in conjunction with close().
	 */
	//virtual bool append(const Data& toCompress, size_t& compressedSize) = 0;

	/** finish compression and receive size of the remaining compressed buffer */
	//virtual bool close(size_t& compressedSize) = 0;


	/**
	 * Compress the original data and save the compressed data in result.
	 * Use this append() function only in conjunction with close(result).
	 */
	virtual bool append(const Data& toCompress, std::vector<char>& result) = 0;

	/** finish compression and receive the remaining compressed buffer */
	virtual bool close(std::vector<char>& result) = 0;

	/** reset the compressor to its initial state */
	virtual void reset() = 0;

private:

	/** the (display) name of the compressor */
	std::string name;

};

#endif /* COMPRESSOR_H_ */
