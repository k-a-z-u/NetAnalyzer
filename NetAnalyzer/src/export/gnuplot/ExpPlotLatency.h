/*
 * ExpPlotLatency.h
 *
 *  Created on: Apr 18, 2013
 *      Author: kazu
 */

#ifndef EXPPLOTLATENCY_H_
#define EXPPLOTLATENCY_H_

#include "../EventExporter.h"
#include "../../http/HTTPListener.h"
#include "../../analyzer/source/NetSourceListener.h"

#include <map>
#include <iosfwd>

class NetSource;

/**
 * export each request->response latency in a per-content-type file
 * to draw latency distribution graphs for each content-type
 */
class ExpPlotLatency : public EventExporter, public HTTPListener, public NetSourceListener {

public:

	/** ctor */
	ExpPlotLatency();

	/** dtor */
	virtual ~ExpPlotLatency();


	void onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& req) override;

	void onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& req, const HTTPHeader& resp, uint32_t latency) override;

	void onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) override;

	void onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) override;

	void onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) override;

	void onHttpClose(const HTTPDecoder& dec) override;


	void onNewTcpConnection(HTTPAnalyzer& analyzer, HTTPDecoder& dec) override;


protected:

	void visit(NetSource& src) override;

private:

	/** hidden copy ctor */
	ExpPlotLatency(const ExpPlotLatency&);

	/** open a file for the given content-type */
	void open(const std::string& contentType);

	/** append one latency information */
	void append(const std::string& contentType, uint32_t latency);

	/** map each content-type to a file */
	std::map<std::string, std::ofstream*> files;

};

#endif /* EXPPLOTLATENCY_H_ */
