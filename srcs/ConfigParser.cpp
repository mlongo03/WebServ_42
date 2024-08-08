#include "ConfigParser.hpp"
#include "Location.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser &src)
{
	*this = src;
}


ConfigParser &ConfigParser::operator=(const ConfigParser &rhs)
{
	(void)rhs;
	return *this;
}

void ConfigParser::parseLine(const std::string &line, bool inServerBlock, bool inLocationBlock, Server &currentServer, Location &currentLocation)
{
	// Ignore comment lines
	if (line.empty() || line[0] == '#')
	{
		return;
	}
	// size_t spacePos = line.find(' '); // here i search for a space after a directive
	size_t spacePos = line.find_first_of(" \t\n\r\f\v"); // now changed to all kind of spaces
	if (spacePos == std::string::npos)
	{
		throw std::runtime_error("Invalid line format");
	}

	std::string key = line.substr(0, spacePos);
	std::string value = line.substr(spacePos + 1);

	// trow error if value is empty
	if (value == ";")
	{
		throw std::runtime_error("Value is empty");
	}

	// Remove the semicolon at the end
	if (value[value.size() - 1] == ';')
	{
		value.erase(value.size() - 1);
	}
	else
	{
		throw std::runtime_error("Line does not end with ';'");
	}

	if (inServerBlock && !inLocationBlock)
	{

		if (key == "listen")
		{
			value = trim(value);
			if (is_valid_port(value))
				currentServer.setListen(value);
			else
				throw std::runtime_error("Invalid port number: " + value);
		}
		else if (key == "server_name")
		{
			std::vector<std::string> serverNames;
			std::istringstream iss(value);
			for (std::string serverName; iss >> serverName;)
			{
				serverNames.push_back(serverName);
			}
			currentServer.setServerNames(serverNames);
		}
		else if (key == "root")
		{
			value = trim(value);
			currentServer.setRoot(value);
		}
		else if (key == "host")
		{
			value = trim(value);
			currentServer.setHost(value);
		}
		else if (key == "index")
		{
			value = trim(value);
			currentServer.setIndex(value);
		}
		else
			throw std::runtime_error("Unknown key in server block: " + key);
	}
	else if (inLocationBlock)
	{
		if (key == "index")
		{
			currentLocation.setIndex(value);
		}
		else if (key == "allow")
		{
			std::vector<std::string> allows;
			std::istringstream iss(value);
			std::string token;
			while (iss >> token)
			{
				if (isValidHttpMethod(token))
					allows.push_back(token);
				else
					throw std::runtime_error("Invalid HTTP method: " + token);
			}
			currentLocation.setAllow(allows);
		}
		else if (key == "autoindex")
		{
			value = trim(value);
			if (value == "on")
				currentLocation.setAutoindex(true);
			else if (value == "off")
				currentLocation.setAutoindex(false);
			else
				throw std::runtime_error("Invalid value for autoindex: " + value);
		}
		else if (key == "root")
		{
			value = trim(value);
			currentLocation.setRoot(value);
		}
		else
		{
			throw std::runtime_error("Unknown key in location block: " + key);
		}
	}
	else
	{
		throw std::runtime_error("Line not in server block or location block");
	}
}

std::vector<Server> ConfigParser::parseServers(const std::vector<std::string> &lines)
{
	std::vector<Server> servers;
	Server currentServer;
	Location currentLocation;
	bool inServerBlock = false;
	bool inLocationBlock = false;

	for (size_t i = 0; i < lines.size(); ++i)
	{
		std::string line = lines[i];

		// Ignore comments and empty lines
		if (line.empty() || line[0] == '#')
		{
			continue;
		}

		// Remove leading and trailing spaces
		line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
		line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

		try
		{
			if (line == "server {")
			{
				// std::cout << "entered server block" << std::endl;
				inServerBlock = true;
				currentServer = Server();
			}
			else if (line.substr(0, 9) == "location ") // Check if the line starts a location block
			{
				inLocationBlock = true;
				currentLocation = Location();

				// size_t spacePos = line.find(' ');
				size_t spacePos = line.find_first_of(" \t\n\r\f\v"); //changed to all kind of spaces
				std::string value = line.substr(spacePos + 1, line.size() - spacePos - 2); // Get the value
				value = trim(value);

				// todo check better this condition
				if (value.empty())
				{
					throw std::runtime_error("Invalid location path");
				}
				else if (value[0] != '/')
				{
					throw std::runtime_error("Location path must start with /");
				}
				else
					currentLocation.setPath(value); // Set the path of the location
			}
			else if (line == "}")
			{

				if (inLocationBlock)
				{
					// std::cout << "pushing location" << std::endl;
					currentServer.locations.push_back(currentLocation);
					inLocationBlock = false;
				}
				else if (inServerBlock)
				{
					servers.push_back(currentServer);
					inServerBlock = false;
				}
			}
			else
			{
				// Parse the line based on whether it's in a server block or a location block
				parseLine(line, inServerBlock, inLocationBlock, currentServer, currentLocation);
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error parsing server block: " << e.what() << std::endl;
			// If an error occurs, skip the rest of the current server block
			while (i < lines.size() && lines[i] != "}")
			{
				++i;
			}
			inServerBlock = false;
			inLocationBlock = false;
		}
	}
	return servers;
}

std::vector<Server> ConfigParser::parseConfig(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		throw std::runtime_error("Could not open file");
	}

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(file, line))
	{
		lines.push_back(line);
	}

	return parseServers(lines);
}
