#include "Location.hpp"

Location::Location() : path("/"),
					   index("index.html"), // if no index is specified in the location block
					   autoindex(false)
{}

Location::~Location() {}

Location::Location(const Location &src)
{
	*this = src;
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		path = rhs.path;
		index = rhs.index;
		allow = rhs.allow;
		autoindex = rhs.autoindex;
		root = rhs.root;
		_cgi_extension = rhs._cgi_extension;
	}
	return *this;
}

//getters

std::string Location::getIndex() const
{
	return index;
}

std::string Location::getPath() const
{
	return path;
}

bool Location::getAutoindex() const
{
	return autoindex;
}

std::string Location::getRoot() const
{
	return root;
}

std::vector<std::string> Location::getAllow() const
{
	return allow;
}

std::vector<std::string> Location::getCgiExtension() const
{
	return _cgi_extension;
}


//setters
void Location::setIndex(const std::string &index)
{
	this->index = index;
}

void Location::setPath(const std::string &path)
{
	this->path = path;
}

void Location::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

void Location::setRoot(const std::string &root)
{
	this->root = root;
}

void Location::setAllow(const std::vector<std::string> &allow)
{
	this->allow = allow;
}

void Location::setCgiExtension(const std::vector<std::string>  &cgi_extension)
{
	this->_cgi_extension = cgi_extension;
}


std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "    Location: |" << location.getPath() << "|" << std::endl;
	os << "      index: |" << location.getIndex() << "|" << std::endl;
	os << "      autoindex: |" << location.getAutoindex() << "|" << std::endl;
	os << "      root: |" << location.getRoot() << "|" << std::endl;

	// os << "      cgi_extension: ";
	// std::vector<std::string> cgi_extension = location.getCgiExtension();
	// for (std::vector<std::string>::const_iterator it = cgi_extension.begin(); it != cgi_extension.end(); ++it) {
	// 	const std::string &name = *it;
	// 	os << "|" << name << "| ";
	// }
	// os << std::endl;
	os << "      allow: ";
	std::vector<std::string> allow = location.getAllow();
	for (std::vector<std::string>::const_iterator it = allow.begin(); it != allow.end(); ++it) {
		const std::string &name = *it;
		os << "|" << name << "| ";
	}
	os << std::endl;

	return os;
}
