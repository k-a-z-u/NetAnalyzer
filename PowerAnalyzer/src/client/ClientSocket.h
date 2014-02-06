#ifndef PA_CLIENT_SOCKET_H
#define PA_CLIENT_SOCKET_H

#include "../common/Socket.h"

#include <string>

namespace PowerAnalyzer {

class ClientSocket : public Socket {
public:
	static const size_t RECV_BUF_SIZE = 1024 * 4; // let the client receive in 4KiB chunks

	ClientSocket();
	virtual ~ClientSocket();

	ClientSocket(ClientSocket&) = delete;
	ClientSocket& operator=(ClientSocket&) = delete;

	void connect(const std::string& server, uint16_t port);
};

} // namespace PowerAnalyzer

#endif // PA_CLIENT_SOCKET_H
