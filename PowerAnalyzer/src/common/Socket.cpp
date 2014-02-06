#include "Socket.h"

#include <unistd.h>

namespace PowerAnalyzer {

Socket::Socket() {
	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		throw "Failed to create socket.";
	}

	int on = 1;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
		throw "Failed to set socket options.";
	}
}

Socket::Socket(const int sock) : sock(sock) {

}

Socket::Socket(Socket&& other) : sock(other.sock) {
	other.sock = -1;
}

Socket::~Socket() {
	if (sock != -1) {
		::close(sock);
	}
}

void Socket::send(const std::vector<char>& data) {
	send(data.data(), data.size());
}

void Socket::send(const void* const data, const size_t len) {
	const auto rc = ::send(sock, data, len, MSG_NOSIGNAL);
	if (rc == -1) {
		throw "Failed to send data over the server socket.";
	}
}

std::vector<char> Socket::recv(const size_t bytes) {
	std::vector<char> data(bytes);

	size_t bytesLeft = bytes;

	while (bytesLeft) {
		const auto offset = bytes - bytesLeft;
		const auto rc = ::recv(sock, data.data() + offset, bytesLeft, 0);
		if (rc == -1) {
			throw "Failed to receive data over the server socket.";
		}
		bytesLeft -= rc;
	}

	return data;
}

} // namespace PowerAnalyzer
