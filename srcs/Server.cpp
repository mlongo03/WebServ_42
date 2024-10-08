#include "Server.hpp"
#include "Location.hpp"

Server::Server() :
	host("0.0.0.0"),
	listen("8080"),
	root("./var/www"),
	_error_page_400("/error_page/400.html"),
	_error_page_401("/error_page/401.html"),
	_error_page_403("/error_page/403.html"),
	_error_page_404("/error_page/404.html"),
	_error_page_405("/error_page/405.html"),
	_error_page_413("/error_page/413.html"),
	_error_page_414("/error_page/414.html"),
	_error_page_500("/error_page/500.html"),
	_error_page_502("/error_page/502.html"),
	_error_page_503("/error_page/503.html"),
	_client_max_body_size(1000), // default client_max_body_size 1000 kilobytes (1000 * 1024 bytes)
	_autoindex(false)
{
	this->_allow.push_back("GET");
	this->_allow.push_back("POST");
	this->_allow.push_back("DELETE");
}

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
		_autoindex = rhs._autoindex;
		_error_page_400 = rhs._error_page_400;
		_error_page_401 = rhs._error_page_401;
		_error_page_403 = rhs._error_page_403;
		_error_page_404 = rhs._error_page_404;
		_error_page_405 = rhs._error_page_405;
		_error_page_413 = rhs._error_page_413;
		_error_page_414 = rhs._error_page_414;
		_error_page_500 = rhs._error_page_500;
		_error_page_502 = rhs._error_page_502;
		_error_page_503 = rhs._error_page_503;
		_client_max_body_size = rhs._client_max_body_size;
		_cgi_extension = rhs._cgi_extension;
		_allow = rhs._allow;
	}
		return *this;
};

bool Server::equalLocations(std::vector<Location> locationss) {
	if (locationss.size() == this->locations.size()) {
		for (size_t i = 0; i < this->locations.size(); i++) {
			if (std::find(locationss.begin(), locationss.end(), this->locations[i]) == locationss.end()) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Server::operator==(const Server& server) {
	return (this->host == server.host && this->index == server.index
			&& this->listen == server.listen && equalLocations(server.locations));
}

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

std::string Server::getErrorPage405() const {
	return _error_page_405;
}

std::string Server::getErrorPage413() const {
	return _error_page_413;
}

std::string Server::getErrorPage414() const {
	return _error_page_414;
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

size_t Server::getClientMaxBodySize() const {
	return _client_max_body_size;
}

std::vector<std::string> Server::getCgiExtension() const {
	return _cgi_extension;
}

bool Server::getAutoindex() const
{
	return _autoindex;
}

std::vector<std::string> Server::getAllow() const {
	return _allow;
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

void Server::setErrorPage405(const std::string& _error_page_405) {
	this->_error_page_405 = _error_page_405;
}

void Server::setErrorPage413(const std::string& _error_page_413) {
	this->_error_page_413 = _error_page_413;
}

void Server::setErrorPage414(const std::string& _error_page_414) {
	this->_error_page_414 = _error_page_414;
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

void Server::setClientMaxBodySize(size_t& _client_max_body_size) {
	this->_client_max_body_size = _client_max_body_size;
}

void Server::setCgiExtension(const std::vector<std::string>& _cgi_extension) {
	this->_cgi_extension = _cgi_extension;
}

void Server::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

void Server::setAllow(const std::vector<std::string>& _allow) {
	this->_allow = _allow;
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
	os << "  autoindex: |" << (server.getAutoindex() == true ? "on" : "off") << "|" << "\n";

	os << "  allow: ";
	std::vector<std::string> allow = server.getAllow();
	for (std::vector<std::string>::const_iterator it = allow.begin(); it != allow.end(); ++it) {
		const std::string &name = *it;
		os << "|" << name << "| ";
	}
	os << std::endl;

	os << "  cgi_extension: ";
	std::vector<std::string> cgiExtensions = server.getCgiExtension();
	for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
		os << "|" << *it << "|";
	}
	os << "\n";
	os << "  error_page_400: |" << server.getErrorPage400() << "|" << "\n";
	os << "  error_page_401: |" << server.getErrorPage401() << "|" << "\n";
	os << "  error_page_403: |" << server.getErrorPage403() << "|" << "\n";
	os << "  error_page_404: |" << server.getErrorPage404() << "|" << "\n";
	os << "  error_page_405: |" << server.getErrorPage405() << "|" << "\n";
	os << "  error_page_413: |" << server.getErrorPage413() << "|" << "\n";
	os << "  error_page_414: |" << server.getErrorPage414() << "|" << "\n";
	os << "  error_page_500: |" << server.getErrorPage500() << "|" << "\n";
	os << "  error_page_502: |" << server.getErrorPage502() << "|" << "\n";
	os << "  error_page_503: |" << server.getErrorPage503() << "|" << "\n";
	os << "  client_max_body_size: |" << server.getClientMaxBodySize() << "|" << "\n";
	std::vector<Location> locations = server.getLocations();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		os << *it << ' ';
	}

	return os;
}


