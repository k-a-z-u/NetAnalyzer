#include "ServerSocket.h"

#include <sys/socket.h>

namespace PowerAnalyzer {

ServerSocket::ServerSocket() : addrin() {

}

ServerSocket::~ServerSocket() {

}

void ServerSocket::bind(const uint16_t port) {
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = INADDR_ANY;
	addrin.sin_port = htons(port);

	if (::bind(sock, (sockaddr*)&addrin, sizeof(addrin)) == -1) {
		throw "Failed to bind server socket.";
	}
}

void ServerSocket::listen() {
	if (::listen(sock, MAX_SERVER_CONNECTIONS) == -1) {
		throw "Server socket failed to start listening for connection.";
	}
}

Socket ServerSocket::accept() {
	socklen_t addrinLen = sizeof(addrin);
	const int newSock = ::accept(sock, (sockaddr*)&addrin, &addrinLen);

	if (newSock < 0) {
		throw "Server socket failed to accept connection.";
	}

	return { newSock };
}

} // namespace PowerAnalyzer
