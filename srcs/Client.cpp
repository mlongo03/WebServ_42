#include "Client.hpp"

Client::Client() : fd(-1), host(""), port(""), server(NULL) {}

Client::~Client() {
}

Client::Client(int fd, std::string host, std::string port, Socket socket)
    : fd(fd), host(host), port(port), socket(socket), server(NULL){}

Client::Client(int fd, std::string host, std::string port, Socket socket, Server *server)
    : fd(fd), host(host), port(port), socket(socket), server(server) {}

Client::Client(const Client &client)
    : fd(client.fd), host(client.host), port(client.port), socket(client.socket), server(client.server) {}

Client &Client::operator=(const Client &client) {
    if (this != &client) {
        fd = client.fd;
        host = client.host;
        port = client.port;
        socket = client.socket;
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

Socket Client::getSocket() const {
    return socket;
}

std::string Client::getRequest() const {
    return request;
}

std::string Client::getResponse() const {
    return response;
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

void Client::setSocket(Socket socket) {
    this->socket = socket;
}

void Client::setRequest(std::string request) {
    this->request = request;
}

void Client::setResponse(std::string response) {
    this->response = response;
}

bool Client::hasServer() const {
    return server != NULL;
}

bool	Client::operator==(int socket)
{
	if (this->fd == socket)
		return true;
	return false;
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
    os << "Client[fd=" << client.getFd()
       << ", host=" << client.getHost()
       << ", port=" << client.getPort()
       << ", server=" << (client.hasServer() ? "set" : "unset")
       << "]";
    return os;
}

