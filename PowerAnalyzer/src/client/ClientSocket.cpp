#include "ClientSocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <cstring>

namespace PowerAnalyzer {

ClientSocket::ClientSocket() {

}

ClientSocket::~ClientSocket() {

}

void ClientSocket::connect(const std::string& server, const uint16_t port) {
	addrinfo hints;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* res;
	if (::getaddrinfo(server.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
		throw "Invalid server argument.";
	}

	bool success = false;
	for (addrinfo* info = res; info; info = info->ai_next) {

		if (::connect(sock, info->ai_addr, info->ai_addrlen) != -1) {
			success = true;
			break;
		}
	}
	freeaddrinfo(res);

	if (!success) {
		throw "Failed to connect the client socket.";
	}
}

} // namespace PowerAnalyzer
