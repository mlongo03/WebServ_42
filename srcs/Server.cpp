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

void Server::addLocation(const Location& location) {
	locations.push_back(location);
}

std::ostream& operator<<(std::ostream& os, const Server& server) {
	os << "\nServer names: ";
	std::vector<std::string> serverNames = server.getServerNames();
	for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
		os << "|" << *it << "|";
	}
	os << "\n";
	os << "  host: |" << server.getHost() << "|" << "\n";
	os << "  listen: |" << server.getListen() << "|" <<  "\n";
	os << "  index: |" << server.getIndex() << "|" << "\n";
	os << "  root: |" << server.getRoot() << "|" << "\n";


	std::vector<Location> locations = server.getLocations();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		os << *it << ' ';
	}

	return os;
}


