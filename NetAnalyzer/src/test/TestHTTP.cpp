/*
 * TestHTTP.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: kazu
 */




#ifdef WITH_TESTS

#include "Test.h"
#include "../http/HTTPDecoder.h"
#include "../http/HTTPListener.h"
#include "../http/HTTPHeader.h"
#include "../buffer/DataBuffer.h"
#include "../network/Packet.h"

#define TEST_HTTP_PAYLOAD(str)	Payload((uint8_t*)str, sizeof(str) - 1)

struct HttpState {
	std::string ctReq;
	std::string ctResp;
	std::string ctTrafReq;
	std::string ctTrafResp;
	const HTTPHeader* headReq;
	const HTTPHeader* headResp;
	DataBuffer bufferReq;
	DataBuffer bufferResp;
	HttpState() : ctReq(), ctResp(), ctTrafReq(), ctTrafResp(),
			headReq(nullptr), headResp(nullptr), bufferReq(), bufferResp() {;}
};

/** http listener for all tests */
class MyHttpListener : public HTTPListener {

	HttpState& state;
public: MyHttpListener(HttpState& state) : state(state) {;}

void onHttpRequest(const HTTPDecoder& dec, const HTTPHeader& req) override {
	(void) dec;
	state.headReq = &req;
	state.ctReq = req.getContentType();
}

void onHttpResponse(const HTTPDecoder& dec, const HTTPHeader& req, const HTTPHeader& resp, uint32_t latency) override {
	(void) dec;
	(void) req;
	(void) latency;
	state.headResp = &resp;
	state.ctResp = resp.getContentType();
}

void onHttpRequestTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const uint8_t* data, uint32_t length) override {
	(void) dec;
	state.bufferReq.append(data, length);
	state.ctTrafReq = reqHeader.getContentType();
}

void onHttpResponseTraffic(const HTTPDecoder& dec, const HTTPHeader& reqHeader, const HTTPHeader& respHeader, const uint8_t* data, uint32_t length) override {
	(void) dec;
	(void) reqHeader;
	state.bufferResp.append(data, length);
	state.ctTrafResp = respHeader.getContentType();
}

void onHttpDone(const HTTPDecoder& dec, const HTTPHeader& header) override {
	(void) dec;
	(void) header;
}

void onHttpClose(const HTTPDecoder& dec) override {
	(void) dec;
}

};

/** test proper handling of head requests */
TEST(HTTP, HeadRequest) {

	HttpState state;
	MyHttpListener listener(state);
	HTTPDecoder dec;
	dec.addListener(listener);

	// send head request
	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("HEAD /index.html HTTP1.0\r\n\r\n") );

	// check
	ASSERT_NOT_NULL(state.headReq);
	ASSERT_NULL(state.headResp);
	ASSERT_STREQ("/index.html", state.headReq->getURL().c_str());
	ASSERT_EQ(HTTP_METHOD_HEAD, state.headReq->getMethod());

	// send head response with content-length
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\nContent-Length: 20\r\n\r\n") );

	// check
	ASSERT_NOT_NULL(state.headResp);

	// send another request. now using GET
	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("GET /index2.html HTTP1.0\r\n\r\n") );

	// check
	ASSERT_STREQ("/index2.html", state.headReq->getURL().c_str());
	ASSERT_EQ(HTTP_METHOD_GET, state.headReq->getMethod());


	// send response with payload
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n") );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("0123456789") );

	// check
	ASSERT_EQ(10, state.bufferResp.bytesUsed());
	ASSERT_DATA_EQ("0123456789", state.bufferResp.get().data, 10);

}

/** this should not crash */
TEST(HTTP, Corrupted1) {

	HttpState state;
	MyHttpListener listener(state);
	HTTPDecoder dec;
	dec.addListener(listener);

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("XYZ \r\n\r\n")  );

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD(" asdfas \r\n\r\n")  );

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n")  );

	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("GET /index.html HTTP1.0\r\n\r\n") );

	// both directions should be closed now
	ASSERT_NULL(state.headReq);
	ASSERT_NULL(state.headResp);

}

/** ignore short headers */
TEST(HTTP, HeaderTooShort) {

	HttpState state;
	MyHttpListener listener(state);
	HTTPDecoder dec;
	dec.addListener(listener);

	ASSERT_NULL(state.headReq);

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("GET /index.html\r\n\r\n") );

	ASSERT_NOT_NULL(state.headReq);
	state.headReq = nullptr;

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("GET \r\n\r\n") );

	ASSERT_NULL(state.headReq);

}

TEST(HTTP, Chunked) {

	HttpState state;
	MyHttpListener listener(state);
	HTTPDecoder dec;
	dec.addListener(listener);

	// send request header
	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("GET /index.html\r\ncontent-length: 0\r\n\r\n"));

	// send response header
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\nTraNsFer-EncoDInG: chUnkEd\r\n\r\n")  );

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("8\r\n")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("12345678")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("\r\n")  );

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("4\r\n")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("ABCD")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("\r\n")  );

	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("0\r\n")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("\r\n")  );

	ASSERT_DATA_EQ("12345678ABCD", state.bufferResp.get().data, 12);

	// send request header
	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("GET /index.html\r\ncontent-length: 0\r\n\r\n"));

	// send another header
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\nContent-LENGTH: 8\r\n\r\n")  );
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("ABCDEFGH")  );

	//ASSERT_DATA_EQ("ABCDEFGH", state.bufferResp.get().data, 8);

}

TEST(HTTP, Interface1) {

	HttpState state;
	MyHttpListener listener(state);
	HTTPDecoder dec;
	dec.addListener(listener);

	// send request header
	dec.onDataFromInitiator( TEST_HTTP_PAYLOAD("GET /index.html\r\ncontent-length: 0\r\n\r\n"));

	// send response header
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("HTTP/1.0 200 OK\r\ncontent-type: test\r\ncontent-length: 4\r\n\r\n")  );

	// check
	ASSERT_STREQ("test", state.ctResp.c_str());
	ASSERT_TRUE(state.ctTrafResp.empty());

	// send payload
	dec.onDataToInitiator( TEST_HTTP_PAYLOAD("1234")  );

	// check
	ASSERT_STREQ("test", state.ctTrafResp.c_str());

}

#endif
