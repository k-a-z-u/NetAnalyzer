/*
 * FolderSource.h
 *
 *  Created on: May 15, 2013
 *      Author: kazu
 */

#ifndef FOLDERSOURCE_H_
#define FOLDERSOURCE_H_

#include "../HTTPDataSource.h"

#include "../../misc/File.h"

// forward declarations
class Analyzer;
class HTTPAnalyzer;
class HTTPDecoder;

/**
 * this class acts as a HTTPDataSoruce and sends all files within
 * one folder to the Analyzer.
 */
class FolderSource : public HTTPDataSource {

public:

	/** ctor */
	FolderSource(File& folder, uint32_t chunkSize);

	/** dtor */
	virtual ~FolderSource();


	void accept(Analyzer& analyzer) override;

	void accept(Exporter& exp) override;

	void start() override;

	void stop() override;

private:

	/** send all files to the analyzer */
	void sendFiles();

	/** send all files in this folder and run subfolders in recursion */
	void sendFolder(File folder, HTTPDecoder& dec);

	/** send one file to the anaylzer */
	void sendFile(File file, HTTPDecoder& dec);

	/** the analyzer to send the files to */
	Analyzer* analyzer;

	/** the folder to read */
	File folder;

	/** true when started else false */
	bool running;

	/** the chunk-size for file-IO */
	int chunkSize;

	/** the buffer to use for file-IO */
	uint8_t* buffer;

};

#endif /* FOLDERSOURCE_H_ */
