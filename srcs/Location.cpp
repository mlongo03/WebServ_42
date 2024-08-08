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
	}
	return *this;
}

std::vector<std::string> Location::getAllow() const
{
	return allow;
}

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

void Location::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

void Location::setRoot(const std::string &root)
{
	this->root = root;
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
	os << "    Location: |" << location.path << "|" << std::endl;
	os << "      index: |" << location.index << "|" << std::endl;
	os << "      autoindex: |" << location.autoindex << "|" << std::endl;
	if (!location.root.empty()) os << "      root: |" << location.root << "|" << std::endl;
	return os;
}
