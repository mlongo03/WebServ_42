#include "ConfigParser.hpp"
#include "Location.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser &configParser)
{
	(void)configParser;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &configParser)
{
	(void)configParser;
	return *this;
}

// ConfigParser::ConfigParser(const char *filepath) : filepath(filepath) {}

// std::string ConfigParser::readFile(const char *filepath)
// {
// 	int fd = open(filepath, O_RDONLY);
// 	if (fd == -1)
// 	{
// 		std::cerr << "Errore nell'apertura del file: " << strerror(errno) << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	struct stat statbuf;
// 	if (stat(filepath, &statbuf) == -1)
// 	{
// 		std::cerr << "Errore nella chiamata stat: " << strerror(errno) << std::endl;
// 		close(fd);
// 		exit(EXIT_FAILURE);
// 	}

// 	char *buffer = new char[statbuf.st_size + 1];
// 	if (!buffer)
// 	{
// 		std::cerr << "Errore nell'allocazione della memoria" << std::endl;
// 		close(fd);
// 		exit(EXIT_FAILURE);
// 	}

// 	ssize_t bytesRead = read(fd, buffer, statbuf.st_size);
// 	if (bytesRead == -1)
// 	{
// 		std::cerr << "Errore nella lettura del file: " << strerror(errno) << std::endl;
// 		delete[] buffer;
// 		close(fd);
// 		exit(EXIT_FAILURE);
// 	}

// 	buffer[bytesRead] = '\0';

// 	close(fd);

// 	std::string fileContent(buffer);
// 	delete[] buffer;

// 	return fileContent;
// }

void ConfigParser::parseLine(const std::string &line, bool inServerBlock, bool inLocationBlock, Server &currentServer, Location &currentLocation)
{
	// Ignore comment lines
	if (line.empty() || line[0] == '#')
	{
		return;
	}
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos)
	{
		throw std::runtime_error("Invalid line format");
	}

	std::string key = line.substr(0, spacePos);
	std::string value = line.substr(spacePos + 1);

	// std::cout << " line is " << line << std::endl;
	// std::cout << "key: |" << key << "| value: |" << value << "|value back: |" << value.back() << "|"<< std::endl;
	// // Check if the line is the start of a location block
	// if (key == "location" && value.back() == '{')
	// {
	// 	std::cout << "entered location block" << std::endl;
	// 	inLocationBlock = true;
	// 	currentLocation.setIndex(value.substr(0, value.size() - 2)); // Remove the trailing space and {
	// 	return;
	// }

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

		else
		{
			throw std::runtime_error("Unknown key in server block: " + key);
		}
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
				// // Remove all kinds of spaces
				// token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
				// token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);

				if (isValidHttpMethod(token))
				{
				// std::cout << "token is " << token << std::endl;
				allows.push_back(token);
				}
				else
					throw std::runtime_error("Invalid HTTP method: " + token);
			}
			currentLocation.setAllow(allows);
		}
		// else if (key == "allow")
		// {
		// 	std::cout << "entered allow" << std::endl;
		// 	// std::cout << "entered allow" << std::endl;
		// 	std::vector<std::string> allows;
		// 	std::istringstream iss(value);
		// 	std::string token;
		// 	while (std::getline(iss, token, ','))
		// 	{
		// 		// Remove all kinds of spaces
		// 		token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
		// 		token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);

		// 		if (isValidHttpMethod(token))
		// 		{
		// 			std::cout << "token is " << token << std::endl;
		// 			allows.push_back(token);
		// 			currentLocation.setAllow(allows);

		// 			std::cout << currentLocation.getAllow().size() << std::endl;
		// 		}
		// 		else
		// 			throw std::runtime_error("Invalid HTTP method: " + token);
		// 	}
		// }
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
				currentLocation = Location(); // Create a new Location object

				size_t spacePos = line.find(' ');
				std::string value = line.substr(spacePos + 1, line.size() - spacePos - 3); // Get the value
				// std::cout << "value is " << value << std::endl;
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

// void ConfigParser::parseConfig(const std::string &content, std::vector<Server> &servers)
// {
// 	std::vector<std::string> lines;
// 	std::string line;
// 	size_t pos = 0, found;

// 	while ((found = content.find_first_of('\n', pos)) != std::string::npos)
// 	{
// 		lines.push_back(content.substr(pos, found - pos));
// 		pos = found + 1;
// 	}
// 	lines.push_back(content.substr(pos));

// 	Server currentServer;
// 	Location currentLocation;
// 	bool inServerBlock = false;
// 	bool inLocationBlock = false;
// 	for (size_t i = 0; i < lines.size(); ++i)
// 	{
// 		line = lines[i];
// 		// std::cout << line << std::endl;
// 		// Ignore comments and empty lines
// 		if (line.empty() || line[0] == '#')
// 		{
// 			continue;
// 		}
// 		// Remove leading and trailing spaces
// 		line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
// 		line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

// 		if (line == "server {")
// 		{
// 			inServerBlock = true;
// 			currentServer = Server();
// 		}
// 		else if (line == "}")
// 		{
// 			if (inLocationBlock)
// 			{
// 				currentServer.locations.push_back(currentLocation);
// 				inLocationBlock = false;
// 			}
// 			else if (inServerBlock)
// 			{
// 				servers.push_back(currentServer);
// 				inServerBlock = false;
// 			}
// 		}
// 		else if (line.find("location") == 0 && line.find('{') != std::string::npos)
// 		{
// 			inLocationBlock = true;
// 			currentLocation = Location();
// 			size_t pathStart = line.find_first_not_of(" \t\n\r\f\v") + 1;
// 			size_t pathEnd = line.find_last_not_of(" \t\n\r\f\v{");
// 			currentLocation.path = line.substr(pathStart, pathEnd - pathStart);
// 		}
// 		else
// 		{
// 			size_t delimiterPos = line.find(' ');
// 			if (delimiterPos != std::string::npos)
// 			{
// 				std::string directive = line.substr(0, delimiterPos);
// 				std::string value = line.substr(delimiterPos + 1);
// 				// Remove the semicolon at the end
// 				if (!value.empty() && value[value.size() - 1] == ';')
// 				{
// 					value.erase(value.size() - 1);
// 				}
// 				if (inLocationBlock)
// 				{
// 					if (directive == "allow")
// 					{
// 						// std::cout << "entered allow" << std::endl;
// 						std::istringstream iss(value);
// 						std::string token;
// 						while (std::getline(iss, token, ','))
// 						{
// 							// Remove all kinds of spaces
// 							token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
// 							token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);

// 							if (isValidHttpMethod(token)) {
// 								currentLocation.allow.push_back(token);
// 							} else {
// 								throw std::runtime_error("Invalid HTTP method: " + token);
// 							}

// 						}
// 					}
// 					else if (directive == "autoindex")
// 					{
// 						currentLocation.autoindex = value;
// 					}
// 					else if (directive == "root")
// 					{
// 						currentLocation.root = value;
// 					}
// 					else if (directive == "index")
// 					{
// 						currentLocation.index = value;
// 					}
// 				}
// 				else if (inServerBlock)
// 				{
// 					if (directive == "server_name")
// 					{
// 						currentServer.server_name = value;
// 					}
// 					else if (directive == "host")
// 					{
// 						currentServer.host = value;
// 					}
// 					else if (directive == "index")
// 					{
// 						currentServer.index = value;
// 					}
// 					else if (directive == "listen")
// 					{
// 						currentServer.listen = value;
// 					}
// 					else if (directive == "root")
// 					{
// 						currentServer.root = value;
// 					}
// 				}
// 			}
// 		}
// 	}
// }
