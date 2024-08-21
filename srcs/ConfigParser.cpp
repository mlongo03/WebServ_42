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


/**
 * @brief Parses the configuration file and returns a vector of Server objects.
 *
 * This function reads the contents of the specified file and parses it line by line.
 * It throws a std::runtime_error if the file cannot be opened.
 *
 * @param filename The path to the configuration file.
 * @return A vector of Server objects parsed from the configuration file.
 * @throws std::runtime_error if the file cannot be opened.
 */
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


/**
 * @brief Parses the server configurations from the given lines of text.
 *
 * This function takes a vector of strings representing the lines of a configuration file and parses them to extract the server configurations.
 * It returns a vector of Server objects representing the parsed server configurations.
 *
 * @param lines The vector of strings representing the lines of the configuration file.
 * @return std::vector<Server> The vector of Server objects representing the parsed server configurations.
 */
std::vector<Server> ConfigParser::parseServers(const std::vector<std::string> &lines)
{
	std::vector<Server> servers;
	Server currentServer;
	Location currentLocation;
	bool inServerBlock = false;
	bool inLocationBlock = false;
	// reset the counters
	serverBlockCount = 0;
	locationBlockCount = 0;

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
				serverBlockCount++;
				std::cout << "entered server block" << std::endl;
				inServerBlock = true;
				currentServer = Server();
			}
			else if (line.substr(0, 8) == "location") // Check if the line starts a location block
			{
				std::cout<< "entered location block " << std::endl;
				locationBlockCount++;
				inLocationBlock = true;
				currentLocation = Location();

				size_t spacePos = line.find_first_of(" \t\n\r\f\v"); //changed to all kind of spaces
				size_t openBracePos = line.find('{');
				if (spacePos == std::string::npos)
				{
					throw std::runtime_error("Invalid location block no location path found");
				}
				if (openBracePos == std::string::npos)
				{
					throw std::runtime_error("Invalid location block open braces not found");
				}
				// Get the value of the path
				std::string valuePath = line.substr(spacePos, line.size() - spacePos - 1); // Get the value
				valuePath = trimSpaces(valuePath);
				// std::cout << "value of location path is: " << valuePath << std::endl;
				if (valuePath.empty())
				{
					throw std::runtime_error("Invalid location path");
				}
				else if (valuePath[0] != '/')
				{
					throw std::runtime_error("Location path must start with /");
				}
				else
					currentLocation.setPath(valuePath); // Set the path of the location
			}
			else if (line == "}")
			{
				std::cout << "found a bracket closed" << std::endl;
				if (inLocationBlock)
				{
					std::cout << "closing location block" << std::endl;
					currentServer.addLocation(currentLocation); // Add the location to the server
					inLocationBlock = false;
					locationBlockCount--;
				}
				else if (inServerBlock)
				{
					std::cout << "closing server block" << std::endl;
					servers.push_back(currentServer);
					inServerBlock = false;
					serverBlockCount--;
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


/**
 * @brief Parses a single line of configuration and updates the corresponding server and location objects.
 *
 * This function takes a line of configuration as input and updates the server and location objects accordingly.
 * It ignores comment lines and throws exceptions for invalid line formats or empty values.
 * The line is expected to have a key-value pair format, with the key separated from the value by spaces.
 * The value can be a single value or a list of values separated by spaces.
 * The function checks if the line is within a server block or a location block and updates the corresponding objects accordingly.
 *
 * @param line The line of configuration to parse.
 * @param inServerBlock A boolean indicating if the line is within a server block.
 * @param inLocationBlock A boolean indicating if the line is within a location block.
 * @param currentServer The current server object to update.
 * @param currentLocation The current location object to update.
 *
 * @throws std::runtime_error If the line format is invalid, the value is empty, or the line does not end with a semicolon.
 * @throws std::runtime_error If the port number, body size, HTTP method, or file extension is invalid.
 * @throws std::runtime_error If an unknown key is encountered in the server block or location block.
 *
 * @return void
 */
void ConfigParser::parseLine(const std::string &line, bool inServerBlock, bool inLocationBlock, Server &currentServer, Location &currentLocation)
{
	std::cout << "in parsing line is: " << line << std::endl;
	// Ignore comment lines
	if (line.empty() || line[0] == '#')
	{
		return;
	}
	size_t spacePos = line.find_first_of(" \t\n\r\f\v"); // now changed to all kind of spaces

	if (spacePos == std::string::npos)
	{
		throw std::runtime_error("Invalid line format");
	}

	// Get the key and value of the Server from the line
	std::string key = line.substr(0, spacePos);
	std::string value = line.substr(spacePos + 1);

	// trow error if value is empty
	if (value.empty())
	{
		throw std::runtime_error("Value is empty");
	}
	// trow error if value is empty
	if (value == ";")
	{
		throw std::runtime_error("Value is empty, it is only semicolon");
	}

	//if the value does not end with a semicolon, throw an error
	if (value[value.size() - 1] != ';')
	{
		throw std::runtime_error("Line does not end with ';'");
	}

	//trim the semicolon and spaces from the value
	value = trimSemicolons(value);
	value = trimSpaces(value);

	//std::cout << "key is: " << key << std::endl;
	// std::cout << " value trimmed is |" << value << "|" << std::endl;

	if (inServerBlock && !inLocationBlock)
	{
		std::cout << "server block count " << serverBlockCount << std::endl;
		if (key == "listen")
		{
			if (isValidNumber(value))
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
			if (value[0] != '/')
				throw std::runtime_error("value of root in server must start with /");
			currentServer.setRoot(value);
		}
		else if (key == "host")
		{
			currentServer.setHost(value);
		}
		else if (key == "index")
		{
			currentServer.setIndex(value);
		}
		else if (key == "client_max_body_size")
		{
			if (isValidNumber(value))
				currentServer.setClientMaxBodySize(value);
			else
				throw std::runtime_error("Invalid body size: " + value);
		}
		else if (key == "error_page")
		{
			size_t spacePosErr = value.find_first_of(" \t\n\r\f\v");
			if (spacePosErr == std::string::npos)
				throw std::runtime_error("Invalid line format in error_page");
			std::string keyFromError = value.substr(0 , 3); // because ERR code is 3 characters
			std::string valueFromError = value.substr(spacePosErr + 1);
			valueFromError = trimSpaces(valueFromError);
			if (valueFromError.empty())
				throw std::runtime_error("Value is empty in error_page");
			if (valueFromError[0] != '/')
			{
				throw std::runtime_error("value of error pages must start with /");
			}
			if (keyFromError == "400")
				currentServer.setErrorPage400(valueFromError);
			else if (keyFromError == "401")
				currentServer.setErrorPage401(valueFromError);
			else if (keyFromError == "403")
				currentServer.setErrorPage403(valueFromError);
			else if (keyFromError == "404")
				currentServer.setErrorPage404(valueFromError);
			else if (keyFromError == "500")
				currentServer.setErrorPage500(valueFromError);
			else if (keyFromError == "502")
				currentServer.setErrorPage502(valueFromError);
			else if (keyFromError == "503")
				currentServer.setErrorPage503(valueFromError);
			else
				throw std::runtime_error("Invalid error code: " + keyFromError);
		}
		else
			throw std::runtime_error("Unknown key in server block: " + key);
	}
	else if (inLocationBlock)
	{
		std::cout << "location block count " << locationBlockCount << std::endl;
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
			if (value == "on")
				currentLocation.setAutoindex(true);
			else if (value == "off")
				currentLocation.setAutoindex(false);
			else
				throw std::runtime_error("Invalid value for autoindex: " + value);
		}
		else if (key == "root")
		{
			if (value[0] != '/')
				throw std::runtime_error("value of root in location must start with /");
			currentLocation.setRoot(value);
		}
		else if (key == "cgi_extension")
		{
			std::vector<std::string> cgi;
			std::istringstream iss(value);
			std::string token;
			while (iss >> token)
			{
				//std::cout << "token is: " << token << std::endl;
				if (hasValidExtension(token))
					cgi.push_back(token);
				else
					throw std::runtime_error("Invalid file extension: " + token);
			}
			currentLocation.setCgiExtension(cgi);
		}
		else
		{
			std::cout << "test in loc block?" << std::endl;
			throw std::runtime_error("Unknown key in location block: " + key);
		}
	}
	else
	{
		std::cout << "test da nessuna parte?" << std::endl;
		throw std::runtime_error("Line not in server block or location block");
	}

}


