/*
 * CmdLine.h
 *
 *  Created on: Apr 24, 2013
 *      Author: kazu
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <vector>
#include <string>

struct CmdKeyVal {

	/** the key (e.g. --dev) */
	std::string key;

	/** the value (e.g. wlan0) */
	std::string val;

	/** ctor */
	CmdKeyVal(const std::string& key, const std::string& val) : key(key), val(val) {;}

};

/**
 * parses command-line arguments
 */
class CmdLine {

public:

	CmdLine(int argc, char* argv[], const std::string& usage);

	virtual ~CmdLine();

	/**
	 * returns true if the given argument exists.
	 * (e.g. started with "-tests")
	 */
	bool has(const std::string& name) const;

	/**
	 * get the value of the provided name
	 * e.g. name = "-device"
	 * this will return the next entry after "-device"
	 */
	const std::string& get(const std::string& name, bool mandatory = true, const std::string& fallback = "") const;

private:

	/** print usage description */
	void printUsage(const std::string& progName);

	/** append all to vector */
	void append(int argc, char* argv[]);

	/** params */
	std::vector<CmdKeyVal> params;

	/** the usage string to display */
	std::string usage;

};

#endif /* CMDLINE_H_ */
