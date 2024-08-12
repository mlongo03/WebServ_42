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
		_error_page_400 = rhs._error_page_400;
		_error_page_401 = rhs._error_page_401;
		_error_page_403 = rhs._error_page_403;
		_error_page_404 = rhs._error_page_404;
		_error_page_500 = rhs._error_page_500;
		_error_page_502 = rhs._error_page_502;
		_error_page_503 = rhs._error_page_503;
	}
		return *this;
};

//getters methods

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

std::string Server::getErrorPage400() const {
	return _error_page_400;
}

std::string Server::getErrorPage401() const {
	return _error_page_401;
}

std::string Server::getErrorPage403() const {
	return _error_page_403;
}

std::string Server::getErrorPage404() const {
	return _error_page_404;
}

std::string Server::getErrorPage500() const {
	return _error_page_500;
}

std::string Server::getErrorPage502() const {
	return _error_page_502;
}

std::string Server::getErrorPage503() const {
	return _error_page_503;
}


//setters methods


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

void Server::setErrorPage400(const std::string& _error_page_400) {
	this->_error_page_400 = _error_page_400;
}

void Server::setErrorPage401(const std::string& _error_page_401) {
	this->_error_page_401 = _error_page_401;
}

void Server::setErrorPage403(const std::string& _error_page_403) {
	this->_error_page_403 = _error_page_403;
}

void Server::setErrorPage404(const std::string& _error_page_404) {
	this->_error_page_404 = _error_page_404;
}

void Server::setErrorPage500(const std::string& _error_page_500) {
	this->_error_page_500 = _error_page_500;
}

void Server::setErrorPage502(const std::string& _error_page_502) {
	this->_error_page_502 = _error_page_502;
}

void Server::setErrorPage503(const std::string& _error_page_503) {
	this->_error_page_503 = _error_page_503;
}


//overload the << operator to print the server object
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
	os << "  error_page_400: |" << server.getErrorPage400() << "|" << "\n";
	os << "  error_page_401: |" << server.getErrorPage401() << "|" << "\n";
	os << "  error_page_403: |" << server.getErrorPage403() << "|" << "\n";
	os << "  error_page_404: |" << server.getErrorPage404() << "|" << "\n";
	os << "  error_page_500: |" << server.getErrorPage500() << "|" << "\n";
	os << "  error_page_502: |" << server.getErrorPage502() << "|" << "\n";
	os << "  error_page_503: |" << server.getErrorPage503() << "|" << "\n";

	std::vector<Location> locations = server.getLocations();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		os << *it << ' ';
	}

	return os;
}


