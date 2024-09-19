#include "Location.hpp"

Location::Location() : path("/"),
					   autoindex(2)
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
		upload_dir = rhs.upload_dir;
		return_map = rhs.return_map;
	}
	return *this;
}

bool Location::operator==(const Location& location) {
	return (this->allow == location.allow && this->autoindex == location.autoindex
				&& this->index == location.index && this->path == location.path
				&& this->root == location.root);
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

int Location::getAutoindex() const
{
	return autoindex;
}

std::string Location::getRoot() const
{
	return root;
}

std::vector<std::string> Location::getCgiExtension() const
{
	return _cgi_extension;
}

std::string Location::getUploadDir() const
{
	return upload_dir;
}

std::map<int, std::string> Location::getReturnMap() const
{
	return return_map;
}

void Location::setIndex(const std::string &index)
{
	this->index = index;
}

void Location::setPath(const std::string &path)
{
	this->path = path;
}

void Location::setAutoindex(int autoindex)
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

void Location::setUploadDir(const std::string &upload_dir)
{
	this->upload_dir = upload_dir;
}

void Location::setReturnMap(const std::map<int, std::string> &return_map)
{
	this->return_map = return_map;
}


std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "      index: |" << location.getIndex() << "|" << std::endl;
	os << "    Location: |" << location.getPath() << "|" << std::endl;
	os << "      autoindex: |" <<( location.getAutoindex() == true ? "on" : "off" ) << "|" << std::endl;
	os << "      root: |" << location.getRoot() << "|" << std::endl;
	os << "      upload_dir: |" << location.getUploadDir() << "|" << std::endl;
	os << "      return_map: ";
	std::map<int, std::string> return_map = location.getReturnMap();
	for (std::map<int, std::string>::const_iterator it = return_map.begin(); it != return_map.end(); ++it) {
		const std::pair<int, std::string> &pair = *it;
		os << "|" << pair.first << "| |" << pair.second << "| ";
	}
	os << std::endl;
	os << "      allow: ";
	std::vector<std::string> allow = location.getAllow();
	for (std::vector<std::string>::const_iterator it = allow.begin(); it != allow.end(); ++it) {
		const std::string &name = *it;
		os << "|" << name << "| ";
	}
	os << std::endl;
	os << "      cgi_extension: ";
	std::vector<std::string> cgi_extension = location.getCgiExtension();
	for (std::vector<std::string>::const_iterator it = cgi_extension.begin(); it != cgi_extension.end(); ++it) {
		const std::string &name = *it;
		os << "|" << name << "| ";
	}
	os << std::endl;

	return os;
}
