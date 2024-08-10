#include "Client.hpp"

Client::Client() : fd(-1), host(""), port(""), server(NULL) {}

Client::~Client() {
}

Client::Client(int fd, std::string host, std::string port)
    : fd(fd), host(host), port(port), server(NULL) {}

Client::Client(int fd, std::string host, std::string port, Server *server)
    : fd(fd), host(host), port(port), server(server) {}

Client::Client(const Client &client)
    : fd(client.fd), host(client.host), port(client.port), server(client.server) {}

Client &Client::operator=(const Client &client) {
    if (this != &client) {
        fd = client.fd;
        host = client.host;
        port = client.port;
        server = client.server;
    }
    return *this;
}

int Client::getFd() const {
    return fd;
}

std::string Client::getHost() const {
    return host;
}

std::string Client::getPort() const {
    return port;
}

Server* Client::getServer() const {
    return server;
}

void Client::setFd(int fd) {
    this->fd = fd;
}

void Client::setHost(std::string host) {
    this->host = host;
}

void Client::setPort(std::string port) {
    this->port = port;
}

void Client::setServer(Server *server) {
    this->server = server;
}

bool Client::hasServer() const {
    return server != NULL;
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
    os << "Client[fd=" << client.getFd()
       << ", host=" << client.getHost()
       << ", port=" << client.getPort()
       << ", server=" << (client.hasServer() ? "set" : "unset")
       << "]";
    return os;
}

