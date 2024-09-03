#include "Client.hpp"

Client::Client() : fd(-1), host(""), port(""), server(NULL), requestObject(NULL) {}

Client::~Client() {
    if (requestObject != NULL)
        delete requestObject;
}

Client::Client(int fd, std::string host, std::string port, Socket socket)
    : fd(fd), host(host), port(port), socket(socket), server(NULL), requestObject(NULL) {}

Client::Client(const Client &client) {
    *this = client;
}

Client &Client::operator=(const Client &client) {
    if (this != &client) {
        fd = client.fd;
        host = client.host;
        port = client.port;
        socket = client.socket;
        request = client.request;
        response = client.response;
        server = client.server;
        requestObject = client.requestObject;
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

Request* Client::getRequestObject() const {
    return requestObject;
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

void Client::setRequestObject(Request *requestObject) {
    this->requestObject = requestObject;
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

void Client::clearResponse() {
    response.clear();
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

