#include "Location.hpp"

Location::Location() : path(""),
					   index("index.html") // if no index is specified in the location block
{
}

Location::~Location() {}

Location::Location(const Location &location) : path(location.path), index(location.index) , allow(location.allow) {}

Location &Location::operator=(const Location &location)
{
	path = location.path;
	index = location.index;
	allow = location.allow;
	// autoindex = location.autoindex;
	// root = location.root;
	return *this;
}

std::vector<std::string> Location::getAllow()
{
	return allow;
}


void Location::setIndex(const std::string &index)
{
	this->index = index;
}

void Location::setPath(const std::string &path)
{
	this->path = path;
}

void Location::setAllow(const std::vector<std::string> &allow)
{
	this->allow = allow;
}

void Location::print() const
{
	std::cout << "    Location |" << path << "|" << std::endl;
	if (!index.empty())
		std::cout << "      index: |" << index << "|" << std::endl;
	if (!allow.empty())
	{
		std::cout << "    allow: ";
		for (std::vector<std::string>::const_iterator it = allow.begin(); it != allow.end(); ++it)
		{
			const std::string &name = *it;
			std::cout << "|" << name << "| ";
		}
		std::cout << std::endl;
	}
	// if (!autoindex.empty()) std::cout << "    autoindex: " << autoindex << std::endl;
	// if (!root.empty()) std::cout << "    root: " << root << std::endl;
}

std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "    Location: " << location.path << std::endl;
	os << "      index: " << location.index << std::endl;
	// os << "autoindex: " << location.autoindex << std::endl;
	// os << "root: " << location.root << std::endl;
	return os;
}
