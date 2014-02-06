/*
 * Settings.cpp
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#include "globals.h"

#include "analyzer/Analyzer.h"

#include "export/gnuplot/ExpPlotContentType.h"
#include "export/gnuplot/ExpPlotLatency.h"
#include "export/gnuplot/ExpHuffman.h"
#include "export/payload/ExpPayload.h"
#include "export/gnuplot/ExpTraffic.h"
#include "export/summary/ExpSummary.h"

#include "compression/GZipCompress.h"
#include "compression/GZipDecompress.h"

#include "compression/QuickLZCompress.h"
#include "compression/QuickLZDecompress.h"

#include "compression/LZMACompress.h"
#include "compression/LZMADecompress.h"

#include "compression/LZOCompress.h"
#include "compression/LZODecompress.h"

#include "compression/LZ4Compress.h"
#include "compression/LZ4Decompress.h"

#include "compression/HuffmanCompress.h"
#include "compression/own/HuffmanTree.h"
#include "compression/own/HuffmanEncoder.h"
#include "compression/dict/Dict.h"
#include "compression/DictCompress.h"

#include "compression/LZWDictCompress.h"
#include "compression/LZWDictDecompress.h"

#include "compression/lzwDict/LZWDict.h"

#include "config/Config.h"

#include "Settings.h"


void Settings::load(struct globals& globals) {

	// additional settings for the selected GUI
	if (globals.guiName.compare("ncurses") == 0) {
		GUINcurses::get()->setUpdateInterval(Config::get().get("gui.ncurses.updateInterval", false).getValue().asInt(2000));
	}

	// configure export-folder
	if (Config::get().get("exporters", false).isValid()) {
		File expFolder(Config::get().get("exporters.folder", true).getValue().asString(""));
		Exporter::setFolder(expFolder);
	}

	// load all exporters
	Settings::loadExporters(globals);

	// load compressors
	Settings::loadCompressors(globals);

	// analyzer settings
	HTTPAnalyzer::setMaxLatency(Config::get().get("analyzer.maxLatency", false).getValue().asInt(5000));
	HTTPAnalyzer::setStaticDynamicThreshold(Config::get().get("analyzer.staticDynamicThreshold", false).getValue().asInt(50));

}

void Settings::loadExporters(struct globals& globals) {

	// load exporters
	for (auto& it : Config::get().get("exporters", false).getAll("exporter")) {

		// get the typ of exporter to load
		std::string type = it.getAttribute("type", true).asString("");

		if (type.compare("huffmanTree") == 0) {

			// create a new huffman exporter
			auto exp = make_unique<ExpHuffman>(globals.expThread);
			exp->setExportInterval(it.getAttribute("interval", true).asInt(60));

			// add content types
			for (auto& ct : it.getAll("contentType")) {
				std::string contentType = ct.getValue().asString("");
				exp->addContentType(contentType);
			}

			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		} else if (type.compare("plotContentType") == 0) {

			// create a new content-type exporter
			auto exp = make_unique<ExpPlotContentType>(globals.expThread);
			exp->setExportInterval(it.getAttribute("interval", true).asInt(60));

			// add content types
			for (auto& ct : it.getAll("contentType")) {
				std::string contentType = ct.getValue().asString("");
				int bin = ct.getAttribute("bin", true).asInt(0);
				exp->addContentType(contentType, bin);
			}

			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		} else if (type.compare("plotLatency") == 0) {

			// create a new latency exporter
			auto exp = make_unique<ExpPlotLatency>();
			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		} else if (type.compare("summary") == 0) {

			// create a new summary exporter
			auto exp = make_unique<ExpSummary>(globals.expThread);
			exp->setExportInterval(it.getAttribute("interval", true).asInt(60 * 15));
			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		} else if (type.compare("payload") == 0) {

			// create a new huffman exporter
			auto exp = make_unique<ExpPayload>();

			// add content types
			for (auto& ct : it.getAll("contentType")) {
				std::string contentType = ct.getValue().asString("");
				std::string fileExt = ct.getAttribute("fileExt", true).asString("");
				exp->addContentType(contentType, fileExt);
			}

			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		} else if (type.compare("traffic") == 0) {

			// create a new latency exporter
			auto exp = make_unique<ExpTraffic>(globals.expThread);
			exp->setExportInterval(it.getAttribute("interval", true).asInt(60));

			// add content types
			for (auto& ct : it.getAll("contentType")) {
				std::string contentType = ct.getValue().asString("");
				std::string bin = ct.getAttribute("bin", true).asString("");
				exp->addContentType(contentType, bin);
			}

			globals.analyzer->addExporter( *exp );
			globals.exporters.push_back(std::move(exp));

		}

	}

}

void Settings::loadCompressors(struct globals& globals) {

	// load all compressors
	for (auto&& it : Config::get().get("compressors", false).getAll("compressor")) {

		// get the type of compressor to use
		const std::string type = it.getAttribute("type", true).asString("");

		// get the name to assign to the compressor
		const std::string name = it.getAttribute("name", true).asString("");

		// also attach corresponding decompressor?
		bool addDecompressor = it.getAttribute("decompressor", false).asBool(false);


		if (type.compare("gzip") == 0) {
			addGzip(it, globals, name, addDecompressor);

		} else if (type.compare("quicklz") == 0) {
			addQuickLZ(it, globals, name, addDecompressor);

		} else if (type.compare("lzma") == 0) {
			addLZMA(it, globals, name, addDecompressor);

		} else if (type.compare("lzo") == 0) {
			addLZO(it, globals, name, addDecompressor);

		} else if (type.compare("lz4") == 0) {
			addLZ4(it, globals, name, addDecompressor);

		} else if (type.compare("huffman") == 0) {
			addHuffman(it, globals, name, addDecompressor);

		} else if (type.compare("dict") == 0) {
			addDict(it, globals, name, addDecompressor);

		} else if (type.compare("lzwdict") == 0) {
			addLZWDict(it, globals, name, addDecompressor);
		}

	}



}

void Settings::addLZMA(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) ce;
	(void) globals;
	(void) name;
	(void) addDecompressor;

#ifdef WITH_LZMA

	const auto level = ce.getAttribute("level", false).asInt(0);

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: LZMACompress(name=" << name << ", level=" << level << ")");

	// compression
	LZMACompress* compr = new LZMACompress();
	compr->setName(ce.getAttribute("name", true).asString(""));
	compr->setLevel(level);

	// decompression
	Decompressor* decomp = (addDecompressor) ? (new LZMADecompress()) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decomp);

#else // WITH_LZMA
	debug(DBG_MAIN, DBG_LVL_WARN, "LZMACompress is not supported with this build");
#endif // WITH_LZMA

}


void Settings::addGzip(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) ce;
	(void) globals;
	(void) name;
	(void) addDecompressor;

#ifdef WITH_ZLIB

	const auto level = ce.getAttribute("level", false).asInt(Z_DEFAULT_COMPRESSION);
	const auto strategy = ce.getAttribute("strategy", false).asString("default");

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: GZipCompress(name=" << name << ", level=" << level << ", strategy=" << strategy << ")");

	GZipCompress* compr = new GZipCompress();
	compr->setName(name);
	compr->setCompressionLevel(level);

	if (strategy.compare("default") == 0) {
		compr->setStrategy(Z_DEFAULT_STRATEGY);
	} else if (strategy.compare("filtered") == 0) {
		compr->setStrategy(Z_FILTERED);
	} else if (strategy.compare("huffmanonly") == 0) {
		compr->setStrategy(Z_HUFFMAN_ONLY);
	} else if (strategy.compare("rle") == 0) {
		compr->setStrategy(Z_RLE);
	} else if (strategy.compare("fixed") == 0) {
		compr->setStrategy(Z_FIXED);
	}

	// using dictionary?
	if (ce.get("dict", false).isValid()) {
		std::string dict = ce.get("dict", true).getValue().asString("");
		compr->setDictionary(dict);
	}

	// decompressor
	Decompressor* decompr = (addDecompressor) ? (new GZipDecompress()) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decompr);

#else // WITH_ZLIB
	debug(DBG_MAIN, DBG_LVL_WARN, "GZIPCompress is not supported with this build");
#endif // WITH_ZLIB

}

void Settings::addQuickLZ(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) ce;
	(void) globals;
	(void) name;
	(void) addDecompressor;

#ifdef WITH_QUICKLZ

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: QuickLZCompress(name=" << name << ")");

	// compressor
	Compressor* compr = new QuickLZCompress();
	compr->setName(name);

	// decompressor
	Decompressor* decomp = (addDecompressor) ? (new QuickLZDecompress()) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decomp);

#else // WITH_QUICKLZ
	debug(DBG_MAIN, DBG_LVL_WARN, "QuickLZCompress is not supported with this build");
#endif // WITH_QUICKLZ

}

void Settings::addLZO(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) ce;
	(void) globals;
	(void) name;
	(void) addDecompressor;

#ifdef WITH_LZO

	const auto level = ce.getAttribute("level", false).asInt(1);

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: LZOCompress(name=" << name << ", level=" << level << ")");

	// compressor
	LZOCompress* compr = new LZOCompress();
	compr->setName(name);
	compr->setCompressionLevel(level);

	// add corresponding decompressor?
	Decompressor* decomp = (addDecompressor) ? (new LZODecompress()) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decomp);

#else // WITH_LZO
	debug(DBG_MAIN, DBG_LVL_WARN, "LZOCompress is not supported with this build");
#endif // WITH_LZO

}

void Settings::addLZ4(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) ce;
	(void) globals;
	(void) name;
	(void) addDecompressor;

#ifdef WITH_LZ4
	
	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: LZ4Compress(name=" << name << ")");

	// compressor
	Compressor* compr = new LZ4Compress();
	compr->setName(name);

	// add corresponding decompressor?
	Decompressor* decompr = (addDecompressor) ? (new LZ4Decompress()) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decompr);
	
#else // WITH_LZ4
	debug(DBG_MAIN, DBG_LVL_WARN, "LZ4Compress is not supported with this build");
#endif // WITH_LZ4

}

void Settings::addHuffman(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) addDecompressor;

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: Huffman");

	std::string values = ce.get("tree",true).getValue().asString("");

	HuffmanBuilder hb(false);
	hb.loadFromString(values, '\t');

	HuffmanTree* hTree = hb.getTree();
	HuffmanEncoder* hEnc = hTree->getEncoder();
	auto* compr = new HuffmanCompress(hEnc->getLUT());
	compr->setName(name);

	// cleanup
	delete hEnc;
	delete hTree;

	globals.analyzer->addCompressorDecompressorPrototype(compr, nullptr);

}

void Settings::addDict(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) addDecompressor;

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: DICT(name=" << name);

	std::string wordStr = ce.get("dict", true).getValue().asString("");

	auto dict = make_unique<DictFlex>();
	dict->setDict( (const uint8_t*)wordStr.c_str(), wordStr.length());

	auto* compr = new DictCompress(*dict);
	compr->setName(name);
	globals.analyzer->addCompressorDecompressorPrototype(compr, nullptr);

	// move this dictionary into globals-struct for cleanup on exit
	globals.dicts.push_back(std::move(dict));

}

void Settings::addLZWDict(const ConfigEntry& ce, struct globals& globals, const std::string& name, bool addDecompressor) {

	(void) addDecompressor;

	debug(DBG_MAIN, DBG_LVL_INFO, "adding compressor: LZW-DICT(name=" << name);

	std::string dictFile = ce.getAttribute("file", true).asString("");
	uint32_t times = ce.getAttribute("times", false).asInt(1);

	// create a new dictionary
	auto dict = make_unique<LZWDict>();

	// load the file several time (to make the dict-entries longer)
	for (uint32_t i = 0; i < times; ++i) {
		dict->loadFile(dictFile);
	}

	// compressor
	auto* compr = new LZWDictCompress(*dict);
	compr->setName(name);

	// add corresponding decompressor?
	Decompressor* decomp = (addDecompressor) ? (new LZWDictDecompress(*dict)) : (nullptr);

	// add
	globals.analyzer->addCompressorDecompressorPrototype(compr, decomp);

	// move this dictionary into globals-struct for cleanup on exit
	globals.lzwDicts.push_back(std::move(dict));

}


