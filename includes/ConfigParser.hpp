#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Server.hpp"
#include "Location.hpp"
#include "Utils.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

class Server;
class Location;

class ConfigParser
{
private:
	const char *filepath;

public:
	ConfigParser();
	~ConfigParser();
	ConfigParser(const ConfigParser &configParser);
	ConfigParser &operator=(const ConfigParser &configParser);

	// ConfigParser(const char *filepath);
	std::string readFile(const char *filepath);
	void parseLine(const std::string &line, bool inServerBlock, bool inLocationBlock, Server &currentServer, Location &currentLocation);
	std::vector<Server> parseServers(const std::vector<std::string> &lines);
	// void parseConfig(const std::string& content, std::vector<Server>& servers);
	std::vector<Server> parseConfig(const std::string& filename);
};
#endif
