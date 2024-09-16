#include "Socket.hpp"
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <climits>

Socket::Socket() : socketfd(-1) {}

Socket::Socket(const Server& server) {
    int sockfd;
    struct addrinfo hints, *res, *p;
    int yes = 1;
	char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server.getHost().c_str(), server.getListen().c_str(), &hints, &res) != 0) {
        throw std::runtime_error("getaddrinfo error");
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            close(sockfd);
            freeaddrinfo(res);
            throw std::runtime_error("setsockopt error");
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        std::string error = "\033[1;31mfailed to bind socket\nHost : " + server.getHost() + "\nPort : " + server.getListen() + "\033[0m";
        freeaddrinfo(res);
        throw std::runtime_error(error);
    }

    if (listen(sockfd, INT_MAX) == -1) {
        close(sockfd);
        freeaddrinfo(res);
        throw std::runtime_error("listen error");
    }

	void *addr;
	if (p->ai_family == AF_INET) { // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		addr = &(ipv4->sin_addr);
	} else { // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
		addr = &(ipv6->sin6_addr);
	}

	inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	ip = std::string(ipstr);

	this->socketfd = sockfd;
	this->host = server.getHost();
	this->port = server.getListen();
	this->ip = ip;

	std::cout << server << std::endl;

    freeaddrinfo(res);
}

Socket::Socket(const Socket& other)
    : socketfd(other.socketfd), port(other.port), ip(other.ip), host(other.host) {
}

Socket& Socket::operator=(const Socket& other) {
    if (this != &other) {
        socketfd = other.socketfd;
        port = other.port;
        ip = other.ip;
        host = other.host;
    }
    return *this;
}

Socket::~Socket() {
}

int Socket::getSocketFd() const {
    return socketfd;
}

std::string Socket::getPort() const {
    return port;
}

std::string Socket::getIp() const {
    return ip;
}

std::string Socket::getHost() const {
    return host;
}

void Socket::setSocketFd(int fd) {
    socketfd = fd;
}

void Socket::setPort(const std::string &port) {
    this->port = port;
}

void Socket::setIp(const std::string &ip) {
    this->ip = ip;
}

void Socket::setHost(const std::string &host) {
    this->host = host;
}

bool	Socket::operator==(int socket)
{
	if (this->socketfd == socket)
		return true;
	return false;
}
