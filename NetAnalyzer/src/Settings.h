/*
 * Settings.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#include "globals.h"

class Settings {

public:

	/** load and apply settings from config.xml */
	static void load(struct globals& globals);

private:

	static void loadCompressors(struct globals& globals);

	static void loadExporters(struct globals& globals);


	static void addGzip(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addQuickLZ(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addLZMA(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addLZO(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addLZ4(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addHuffman(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addDict(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

	static void addLZWDict(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor);

};

