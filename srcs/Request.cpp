#include "Request.hpp"

Request::Request(const std::string &rawRequest) : start_time(std::time(NULL))
{
	parseRequest(rawRequest);
}

time_t Request::getStartTime() const
{
	return start_time;
}

std::vector<std::string> splitBySpaces(const std::string &str)
{
	std::stringstream ss(str);
	std::string word;
	std::vector<std::string> result;
	while (ss >> word)
	{
		result.push_back(word);
	}
	return result;
}

void Request::parseRequest(const std::string &rawRequest)
{
	std::stringstream requestStream(rawRequest);
	std::string requestLine;

	if (!std::getline(requestStream, requestLine))
	{
		throw InvalidHttpRequestException("Request line is missing");
	}

	std::vector<std::string> parts = splitBySpaces(requestLine);

	if (parts.size() != 3)
	{
		throw InvalidHttpRequestException("Request line must have a method, URI, and HTTP version");
	}


  // std::cout << "method = " << method << ", path = " << path << ", http version = " << httpVersion << std::endl;
	method = parts[0];
	path = parts[1];
	httpVersion = parts[2];

	std::cout << "method = " << method << ", path = " << path << ", http version = " << httpVersion << std::endl;

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		throw InvalidHttpRequestException("Unsupported HTTP method: " + method);
	}

	if (path.empty() || path[0] != '/')
	{
		throw InvalidHttpRequestException("Invalid or missing URI: " + path);
	}

	if (httpVersion != "HTTP/1.1")
	{
		throw InvalidHttpRequestException("Unsupported HTTP version: " + httpVersion);
	}

	// Handle query string parsing
	size_t queryPos = path.find('?');
	if (queryPos != std::string::npos)
	{
		std::string queryString = path.substr(queryPos + 1);
		parseQueryString(queryString);
		path = path.substr(0, queryPos);
	}
}

void Request::parseHeaders(const std::string &rawRequest)
{
	std::stringstream requestStream(rawRequest);
	std::string line;

	// Skip the request line (the first line)
	if (!std::getline(requestStream, line))
	{
		throw InvalidHttpRequestException("Request line is missing");
	}

	// Parse headers
	while (std::getline(requestStream, line) && !line.empty() && line != "\r")
	{
		// Remove the carriage return character if it's there
		if (!line.empty() && line[line.size() - 1] == '\r')
		{
			line.erase(line.size() - 1);
		}

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
		{
			throw InvalidHttpRequestException("Malformed header line: " + line);
		}

		std::string headerName = line.substr(0, colonPos);
		std::string headerValue = line.substr(colonPos + 1);

		// Remove leading and trailing whitespace from the header value
		headerValue.erase(0, headerValue.find_first_not_of(" \t"));
		headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

		headers[headerName] = headerValue;
	}
	std::cout << "headers done \n";
}

void Request::parseQueryString(const std::string &queryString)
{
	std::stringstream ss(queryString);
	std::string item;
	while (std::getline(ss, item, '&'))
	{
		size_t delimiterPos = item.find('=');
		if (delimiterPos != std::string::npos)
		{
			std::string key = item.substr(0, delimiterPos);
			std::string value = item.substr(delimiterPos + 1);
			queryParameters[key] = value;
		}
		else
		{
			queryParameters[item] = "";
		}
	}
}

std::string Request::getMethod() const
{
	return method;
}

std::string Request::getPath() const
{
	return path;
}

std::string Request::getHttpVersion() const
{
	return httpVersion;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return headers;
}

std::string Request::getBody() const
{
	return body;
}

std::string& Request::getBody()
{
	return body;
}

bool fileExistsAndAccessible(const std::string &filename, int mode)
{
	return (access(filename.c_str(), mode) == 0);
}

std::string determineContentType(const std::string &filePath)
{
	size_t dotPosition = filePath.find_last_of(".");
	if (dotPosition != std::string::npos)
	{
		std::string extension = filePath.substr(dotPosition + 1);
		if (extension == "html" || extension == "htm")
		{
			return "text/html";
		}
		else if (extension == "jpg" || extension == "jpeg")
		{
			return "image/jpeg";
		}
		else if (extension == "png")
		{
			return "image/png";
		}
		else if (extension == "gif")
		{
			return "image/gif";
		}
		else if (extension == "css")
		{
			return "text/css";
		}
		else if (extension == "js")
		{
			return "application/javascript";
		}
		else if (extension == "txt")
		{
			return "text/plain";
		}
		else if (extension == "pdf")
		{
			return "application/pdf";
		}
	}
	return "application/octet-stream";
}

Location *Request::checkLocation(Server &server) const
{
	std::vector<Location> locations = server.getLocations();

	for (size_t i = 0; i < locations.size(); i++)
	{
		// std::cout << "Request path : " << path << ", Location path : " << locations[i].getPath() << std::endl;
		if (path.find(locations[i].getPath()) == 0)
		{
			// std::cout << "Location found: " << locations[i].getPath() << std::endl;
			if (path.size() == locations[i].getPath().size() || path[locations[i].getPath().size()] == '/')
			{
				return new Location(locations[i]);
			}
		}
	}
	return NULL;
}

std::string Request::getFilePath(Location *location, Server server) const
{
	if (location)
	{
		if (!location->getRoot().empty())
		{
			return location->getRoot() + path;
		}
		else
		{
			return server.getRoot() + path;
		}
	}
	else
	{
		return server.getRoot() + path;
	}
}

bool containsString(const std::vector<std::string> &vec, const std::string &str)
{
	return std::find(vec.begin(), vec.end(), str) != vec.end();
}

bool Request::checkMethod(Location *location, Server &server, const std::string &methodToCheck) const
{
	return (method == methodToCheck && (location != NULL ? (location->getAllow().size() == 0 ? containsString(server.getAllow(), methodToCheck) : containsString(location->getAllow(), methodToCheck)) : containsString(server.getAllow(), methodToCheck)));
}

std::string Request::generateResponse(Server &server) const
{
	Response response(200, "OK");
	Location *location = checkLocation(server);
	std::string filePath = getFilePath(location, server);
	// std::cout << "Request path: " << path << std::endl;
	// std::cout << "headers of the request are !!!!!!!!!:" << std::endl;
	// printHeaders(headers);
	// std::cout << "!!!!!!!!!!!!!!" << std::endl;
	if (shouldRedirect(location, server))
	{
		if (checkMethod(location, server, "GET") || checkMethod(location, server, "POST") || checkMethod(location, server, "DELETE"))
			handleRedirect(location, server, response);
		else
			handleUnsupportedMethod(server, response);
	}
	else if (checkMethod(location, server, "GET"))
	{
		handleGetRequest(server, response, location, filePath);
	}
	else if (checkMethod(location, server, "POST"))
	{
		handlePostRequest(server, response, location, filePath);
	}
	else if (checkMethod(location, server, "DELETE"))
	{
		handleDeleteRequest(server, response, filePath);
	}
	else
	{
		handleUnsupportedMethod(server, response);
	}
	if (location)
		delete (location);
	return response.generateResponse();
}

bool isDirectory(const std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return s.st_mode & S_IFDIR;
	}
	return false;
}

std::string Request::generateDirectoryListingHTML(const std::string &directoryPath) const
{
	std::ostringstream oss;
	oss << "<html><body><h1>Directory listing for " << directoryPath << "</h1><ul>";

	DIR *dir = opendir(directoryPath.c_str());
	if (dir)
	{
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string name = entry->d_name;
			if (name != "." && name != "..")
			{
				oss << "<li><a href=\"" << path + (path[path.size() - 1] != '/' ? "/" : "") + name << "\">" << name << "</a></li>";
			}
		}
		closedir(dir);
	}
	oss << "</ul></body></html>";
	return oss.str();
}

bool checkIndexExistence(Location *location, Server &server)
{
	return (location != NULL ? (location->getIndex().empty() ? (server.getIndex().empty() ? false : true) : true) : (server.getIndex().empty() ? false : true));
}

std::string getIndex(Location *location, Server &server)
{
	return (location != NULL ? (location->getIndex().empty() ? server.getIndex() : location->getIndex()) : server.getIndex());
}

bool checkAutoindexStatus(Location *location, Server &server)
{
	return (location != NULL ? (location->getAutoindex() == 2 ? server.getAutoindex() : location->getAutoindex()) : server.getAutoindex());
}

bool endsWithAny(const std::string &str, const std::vector<std::string> &suffixes)
{
	for (size_t i = 0; i < suffixes.size(); i++)
	{
		if (str.size() >= suffixes[i].size() && str.rfind(suffixes[i]) == (str.size() - suffixes[i].size()))
		{
			return true;
		}
	}
	return false;
}

bool checkCgiMatch(Location *location, Server &server, std::string filePath)
{
	return (location != NULL ? (location->getCgiExtension().size() == 0 ? (server.getCgiExtension().size() == 0 ? false : endsWithAny(filePath, server.getCgiExtension())) : endsWithAny(filePath, location->getCgiExtension())) : (server.getCgiExtension().size() == 0 ? false : endsWithAny(filePath, server.getCgiExtension())));
}

std::vector<std::string> getCgiExtension(Location *location, Server &server)
{
	return (location != NULL ? (location->getCgiExtension().size() == 0 ? server.getCgiExtension() : location->getCgiExtension()) : server.getCgiExtension());
}

void Request::handleGetRequest(Server &server, Response &response, Location *location, std::string filePath) const
{

	// std::cout << "GET REQUEST FOR file path : " << filePath << std::endl;
	if (isDirectory(filePath))
	{
		if (!fileExistsAndAccessible(filePath, R_OK))
		{
			response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
			return;
		}
		if (checkIndexExistence(location, server))
		{
			std::string tmpfilePath = filePath + (filePath[filePath.size() - 1] != '/' ? "/" : "") + getIndex(location, server);
			if (checkAutoindexStatus(location, server) && (!fileExistsAndAccessible(tmpfilePath, F_OK) || !fileExistsAndAccessible(tmpfilePath, R_OK)))
			{
				std::string directoryListingHTML = generateDirectoryListingHTML(filePath);
				response.setBodyFromString(directoryListingHTML);
				response.setHeader("Content-Type", "text/html");
				return;
			}
			filePath = tmpfilePath;
		}
		else if (checkAutoindexStatus(location, server))
		{
			std::string directoryListingHTML = generateDirectoryListingHTML(filePath);
			response.setBodyFromString(directoryListingHTML);
			response.setHeader("Content-Type", "text/html");
			return;
		}
		else
		{
			response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
			return;
		}
	}

	if (!fileExistsAndAccessible(filePath, F_OK))
	{
		response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
	}
	else if (!fileExistsAndAccessible(filePath, R_OK))
	{
		response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
    } else if (checkCgiMatch(location, server, filePath)) {
        if (isFileEmpty(filePath)) {
            response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
        } else {
            std::vector<std::string> extensions = getCgiExtension(location, server);
            if (!fileExistsAndAccessible(filePath, X_OK)) {
                response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
            } else {
                try
                {
                    Cgi cgiHandler(filePath, extensions, *this);
                    cgiHandler.prepareEnvVars(*this);
                    cgiHandler.execute(response, server, *this);
                }
                catch(const std::exception& e) {
                    response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
                }
            }
        }
	} else {
		std::string contentType = determineContentType(filePath);
		// std::cout << "Content-Type of a normal get: " << contentType << std::endl;
		response.setHeader("Content-Type", contentType);
		std::ifstream file(filePath.c_str(), std::ios::binary);
		std::ostringstream buffer;
		buffer << file.rdbuf();
		response.setBodyFromString(buffer.str());
        file.close();
	}
}

std::string Request::getFilenameFromPath(const std::string& path) const {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == 0) {
        return "";
    }
    if (lastSlash != std::string::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

std::string Request::getFileExtensionFromContentType() const {
    std::map<std::string, std::string> contentTypeMap;
    contentTypeMap.insert(std::make_pair("text/html", ".html"));
    contentTypeMap.insert(std::make_pair("application/json", ".json"));
    contentTypeMap.insert(std::make_pair("text/plain", ".txt"));
    contentTypeMap.insert(std::make_pair("image/jpeg", ".jpg"));
    contentTypeMap.insert(std::make_pair("image/png", ".png"));
    contentTypeMap.insert(std::make_pair("application/pdf", ".pdf"));
    contentTypeMap.insert(std::make_pair("application/xml", ".xml"));
    contentTypeMap.insert(std::make_pair("application/octet-stream", ".bin"));

    std::map<std::string, std::string>::const_iterator it = contentTypeMap.find(getContentType());

    if (it != contentTypeMap.end()) {
        return it->second;
    } else {
        return ".tmp";
    }
}

std::string Request::generateUniqueFilename() const {
    std::stringstream ss;
    std::string fileExtension = getFileExtensionFromContentType();

    ss << "upload_" << std::time(NULL) << "_" << rand() % 10000 << fileExtension;
    return ss.str();
}

void Request::handlePostRequest(Server &server, Response &response, Location *location, std::string filePath) const {
    // Check if it's a CGI request
    if (checkCgiMatch(location, server, filePath)) {
        std::vector<std::string> extensions = getCgiExtension(location, server);

        if (!fileExistsAndAccessible(filePath, F_OK)) {
            response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
            return;
        } else if (!fileExistsAndAccessible(filePath, R_OK) || !fileExistsAndAccessible(filePath, X_OK)) {
            response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
            return;
        } else if (isFileEmpty(filePath)) {
            response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
            return;
        }

        try {
            Cgi cgiHandler(filePath, extensions, *this);
            cgiHandler.prepareEnvVars(*this);
            cgiHandler.execute(response, server, *this);
        } catch (const std::exception &e) {
            response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
        }
        return;
    }

    // checkif  the body of the post request exceeds the max body size
    std::string requestBody = getBody();
    size_t requestBodySize = requestBody.size();
    // std::cout << "Request body size: " << requestBodySize << std::endl;
    if (requestBodySize > server.getClientMaxBodySize())
    {
      // std::cerr << "Request Entity Too Large" << std::endl;
      response.setResponseError(response, server, 413, "Payload Too large", server.getErrorPage413());
      return;
    }

    // If not CGI, check for upload directory in location
    std::string uploadDir = "";
    if (location && !location->getUploadDir().empty()) {
        uploadDir = location->getUploadDir();
    }

    // If upload directory is not specified, return an error
    if (uploadDir.empty()) {
        response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
        return;
    }

    // Ensure the upload directory exists
    if (!fileExistsAndAccessible(uploadDir, F_OK)) {
        if (mkdir(uploadDir.c_str(), 0755) != 0) {
            response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
            return;
        }
    }

    std::string fullPath = uploadDir + path;
    if (getFilenameFromPath(path).empty()) {
        if (path[path.length() - 1] != '/') {
            fullPath += "/";
        }
        fullPath += generateUniqueFilename();
    }
    // std::cout << "upload dir : " << uploadDir << std::endl;
    // std::cout << "full path : " << fullPath << std::endl;

    // Create directories if they don't exist
    size_t pos = 0;
    while ((pos = fullPath.find('/', pos + 1)) != std::string::npos) {
        std::string dir = fullPath.substr(0, pos);
        if (!fileExistsAndAccessible(dir, F_OK)) {
            if (mkdir(dir.c_str(), 0755) != 0) {
                response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
                return;
            }
        }
    }

    // Write the file
    std::ofstream outFile(fullPath.c_str(), std::ios::binary);
    if (outFile) {
        outFile.write(body.c_str(), body.length());
        outFile.close();
        if (outFile) {
            response.setStatusCode(201);
            response.setStatusMessage("Created");
            response.setBodyFromString("<html><body><h1>201 Created</h1><p>File uploaded successfully.</p></body></html>");
            response.setHeader("Content-Type", "text/html");
        } else {
            response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
        }
    } else {
        response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
    }
}

void Request::handleDeleteRequest(Server &server, Response &response, std::string filePath) const {
	// std::cout << "DELETE REQUEST FOR file: " << filePath << std::endl;
	if (isDirectory(filePath))
	{
		response.setResponseError(response, server, 405, "Method Not Allowed", server.getErrorPage405());
		return;
	}
	if (!fileExistsAndAccessible(filePath, F_OK))
	{
		response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
	}
	else if (!fileExistsAndAccessible(filePath, W_OK))
	{
		response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
	}
	else
	{
		if (remove(filePath.c_str()) == 0)
		{
			// this is the expected response for a successful DELETE request
			response.setResponseCode(204);
			response.setStatusMessage("No Content");
			response.setBodyFromString("");
		}
		else
		{
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
		}
	}
}

void Request::handleUnsupportedMethod(Server &server, Response &response) const
{
	response.setResponseError(response, server, 405, "Method Not Allowed", server.getErrorPage405());
}

std::map<std::string, std::string> Request::getQueryParameters() const {
	return queryParameters;
}

void Request::setBody(std::string body)
{
	this->body = body;
}

void Request::printHeaders(const std::map<std::string, std::string> &headers) const
{
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

std::string Request::getContentType() const {
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
    if (it != headers.end()) {
        return trimSpaces(it->second);
    }
    return "";
}

bool Request::shouldRedirect(Location* location, Server& server) const {
    (void)server; // future use now is not needed
    // Check if location is not null and has a redirection rule
    // std::cout << "in shouldRedirect" << std::endl;
    if (location && !location->getReturnMap().empty())
    {
      return true;
    }
    return false;
}

void Request::handleRedirect(Location *location, Server &server, Response &response) const
{
	std::map<int, std::string> returnedMAP = location->getReturnMap();
	std::map<int, std::string>::iterator it = returnedMAP.begin();
	int statusCode = it->first;
	std::string url = it->second;

	// map for all status messages
	std::map<int, std::string> status_message;
	status_message[301] = "Moved Permanently";
	status_message[302] = "Found";
	status_message[303] = "See Other";
	status_message[307] = "Temporary Redirect";
	status_message[308] = "Permanent Redirect";

	// Find the status message
	std::map<int, std::string>::iterator it2 = status_message.find(statusCode);

	// Check if the status code is valid
	if (it2 == status_message.end())
	{
		response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
		return;
	}
	// Set the redirect response
	setRedirectResponse(response, statusCode, it2->second, url);

	return;
}

void Request::setRedirectResponse(Response& response, int statusCode,const std::string& statusMessage, std::string& url) const {
	response.setStatusCode(statusCode);
	response.setStatusMessage(statusMessage);
	response.setHeader("Location", url);
	response.setHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	response.setHeader("Pragma", "no-cache");
	response.setHeader("Expires", "0");
	// 307 and 308 preserve the original method
	if (statusCode == 307 || statusCode == 308)
	{
		response.setHeader("Original-Method", method);
	}
}
