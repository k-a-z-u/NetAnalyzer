#ifndef PA_COMMON_SOCKET_H
#define PA_COMMON_SOCKET_H

#include <netinet/in.h>

#include <vector>

namespace PowerAnalyzer {

class Socket {
public:
	Socket(int sock);
	Socket(Socket&& other);
	virtual ~Socket();

	// disallow copy construction and assigment
	Socket(Socket&) = delete;
	Socket& operator=(Socket&) = delete;

	/** Send data of any size. */
	virtual void send(const std::vector<char>& data);
	virtual void send(const void* data, size_t len);

	/** Receive [bytes] of data.*/
	virtual std::vector<char> recv(size_t bytes);

protected:
	Socket();

	int sock = -1;
};

} // namespace PowerAnalyzer

#endif // PA_COMMON_SOCKET_H
