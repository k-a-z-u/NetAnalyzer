#ifndef PA_SERVER_SOCKET_H
#define PA_SERVER_SOCKET_H

#include "../common/Socket.h"

namespace PowerAnalyzer {

class ServerSocket : public Socket {
public:
	ServerSocket();
	virtual ~ServerSocket();

	ServerSocket(ServerSocket&) = delete;
	ServerSocket& operator=(ServerSocket&) = delete;

	void bind(const uint16_t port);
	void listen();
	Socket accept();

private:
	static const int MAX_SERVER_CONNECTIONS = 2; // The real max is SOMAXCONN.

	sockaddr_in addrin;
};

} // namespace PowerAnalyzer

#endif // PA_SERVER_SOCKET_H
