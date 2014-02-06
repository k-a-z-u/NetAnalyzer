/*
 * Analyzer.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <memory>
#include <vector>

#include "Summary.h"
#include "CompDecomp.h"

// forward declarations
class HTTPDataSource;
class HTTPAnalyzer;
class Decompressor;
class Compressor;
class Exporter;
class GUI;

/**
 * this class will perform the actual analysis of everything.
 *
 * a data-source needs to be set which will then trigger HTTP-analysis
 *
 */
class Analyzer {

public:

	/** ctor */
	Analyzer();

	/** dtor */
	virtual ~Analyzer();



	/** get the summary (data-model) this analyzer produces */
	Summary& getSummary() const;

	/**
	 * add one exporter
	 * the Exporters setModel() and visit() will be triggered
	 */
	void addExporter(Exporter& exporter);

	/**
	 * Add a compressor(+ corresponding decompressor) prototype.
	 *
	 * For each new connection all prototypes will
	 * be used to create new compressors(/decompressors) for this new connection.
	 *
	 * the decompressor MAY be null!
	 *
	 * The function will take ownership of the passed pointer.
	 */
	void addCompressorDecompressorPrototype(Compressor* compressor, Decompressor* decompressor);

	/**
	 * will be triggered from DataProviders whenever a new HTTPConnection occured.
	 * the Analyzer will return a new and properly configured HttpAnalyzer.
	 */
	HTTPAnalyzer* onHttpConnection();

	/**
	 * "attach" a new HttpDataSource to this Analyzer.
	 * (the analyzer will simply call the Source's visitFromAnalyzer() method)
	 */
	void setHTTPDataSource(HTTPDataSource* src);

	/** set the GUI to use. the Analyzer will register itself as Model within the GUI */
	void setGUI(GUI* newGUI);

private:

	/** hidden copy ctor and assignment operator */
	Analyzer(const Analyzer&);
	Analyzer& operator=(const Analyzer&);

	/* attributes */

	/** the gui to use */
	GUI* gui;

	/** store all gathered facts here */
	Summary summary;

	/** the source sending HTTP data to the analyzer */
	HTTPDataSource* srcHttp;

	/**
	 * all available Compressor/Decompressor pairs.
	 * they will be added to each (new) HTTPAnalyzer
	 */
	std::vector<CompressorDecompressor> compressors;

};

#endif /* ANALYZER_H_ */
