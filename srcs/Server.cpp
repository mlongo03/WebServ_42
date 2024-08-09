#include "Server.hpp"
#include "Location.hpp"

Server::Server() :
	host("0.0.0.0"),
	listen("8080"),
	index("index.html") // default index if no index is specified in the location block
{}

Server::~Server()
{}

Server::Server(const Server& src)
{
	*this = src;
}


Server& Server::operator=(const Server& rhs)
{
	if (this != &rhs) {
		server_names = rhs.server_names;
		host = rhs.host;
		listen = rhs.listen;
		root = rhs.root;
		index = rhs.index;
		locations = rhs.locations;
	}
		return *this;
};

std::vector<std::string> Server::getServerNames() const{
	return server_names;
}

std::string Server::getHost() const {
	return host;
}

std::string Server::getListen() const {
	return listen;
}

std::string Server::getRoot() const {
	return root;
}

std::string Server::getIndex() const {
	return index;
}

std::vector<Location> Server::getLocations() const{
	return locations;
}

void Server::setServerNames(const std::vector<std::string>& server_names) {
	this->server_names = server_names;
}

void Server::setHost(const std::string& host) {
	this->host = host;
}

void Server::setListen(const std::string& listen) {
		this->listen = listen;
}

void Server::setRoot(const std::string& root) {
	this->root = root;
}


void Server::setIndex(const std::string& index) {
	this->index = index;
}

std::ostream& operator<<(std::ostream& os, const Server& server) {
	os << "\nServer names: ";
	for (std::vector<std::string>::const_iterator it = server.server_names.begin(); it != server.server_names.end(); ++it) {
		os << "|" << *it << "|";
	}
	os << "\n";
	os << "  host: |" << server.host << "|" << "\n";
	os << "  listen: |" << server.listen << "|" <<  "\n";
	os << "  index: |" << server.index << "|" << "\n";
	os << "  root: |" << server.root << "|" << "\n";

	for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
		os << *it << ' ';
	}

	return os;
}
