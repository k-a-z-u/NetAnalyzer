/*
 * HTTPAnalyzer.h
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */

#ifndef HTTPANALYZER_H_
#define HTTPANALYZER_H_

#include "../http/HTTPListener.h"

#include <functional>
#include <memory>
#include <vector>

struct Summary;
class Compressor;
class Decompressor;
class HTTPDecoder;
class HTTPHeader;
struct HTTPStats;
struct CompressorDecompressor;



/**
 * this class will perform the analysis of HTTP transfers,
 * apply compressors to it and track other statistics.
 */
class HTTPAnalyzer : public HTTPListener {

public:

	/** ctor */
	HTTPAnalyzer(Summary& summary);

	/** dtor */
	~HTTPAnalyzer();

	void onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& reqHeader) override;

	void onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, uint32_t latency) override;

	void onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) override;

	void onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) override;

	void onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) override;

	void onHttpClose(const HTTPDecoder& dec) override;


	/**
	 * Each compression algorithm will calculate and report by how much
	 * the data could be compressed.
	 *
	 * The corresponding Decompressor MAY be null!
	 *
	 * The function will take ownership of the passed pointer.
	 */
	void addCompressorDecompressor(Compressor* comp, Decompressor* decomp);



	/** set the maximum latency to accept (for statistics) */
	static void setMaxLatency(uint32_t max);

	/** set the threshold to distinguish between static and dynamic payload (in milliseconds) */
	static void setStaticDynamicThreshold(uint32_t ms);

private:

	void openCompressors();
	void appendToCompressors(const std::string& type, const uint8_t* data, uint32_t length);
	void closeCompressors(const std::string& type);

	/** the global summary to write the stats to */
	Summary& summary;

	/** use this ref to distinguish stats between static and dynamic payloads */
	HTTPStats* statsByLatency;


	/** all compressor(/decompressor) pairs to pipe the payload through */
	std::vector<CompressorDecompressor> compressors;

	HTTPAnalyzer(const HTTPAnalyzer&);
	HTTPAnalyzer& operator=(const HTTPAnalyzer&);




	/** -------------------------------- SETTINGS FOR ALL INSTANCES -------------------------------- */

	/** skip all latencies above this value */
	static uint32_t maxLatency;

	/** the threshold (in milliseconds) to distinguish between static and dynamic payload */
	static uint32_t staticDynamicThreshold;



};

#endif /* HTTPANALYZER_H_ */
