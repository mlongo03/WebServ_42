#include "Request.hpp"

Request::Request(const std::string& rawRequest)  : start_time(std::time(NULL)){
    parseRequest(rawRequest);
}

time_t Request::getStartTime() const {
    return start_time;
}

std::vector<std::string> splitBySpaces(const std::string& str) {
    std::stringstream ss(str);
    std::string word;
    std::vector<std::string> result;
    while (ss >> word) {
        result.push_back(word);
    }
    return result;
}

void Request::parseRequest(const std::string& rawRequest) {
    std::stringstream requestStream(rawRequest);
    std::string requestLine;

    if (!std::getline(requestStream, requestLine)) {
        throw InvalidHttpRequestException("Request line is missing");
    }

    std::vector<std::string> parts = splitBySpaces(requestLine);

    if (parts.size() != 3) {
        throw InvalidHttpRequestException("Request line must have a method, URI, and HTTP version");
    }

    method = parts[0];
    path = parts[1];
    httpVersion = parts[2];

    std::cout << "method = " << method << ", path = " << path << ", http version = " << httpVersion << std::endl;

    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw InvalidHttpRequestException("Unsupported HTTP method: " + method);
    }

    if (path.empty() || path[0] != '/') {
        throw InvalidHttpRequestException("Invalid or missing URI: " + path);
    }

    if (httpVersion != "HTTP/1.1") {
        throw InvalidHttpRequestException("Unsupported HTTP version: " + httpVersion);
    }

    // Handle query string parsing
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        std::string queryString = path.substr(queryPos + 1);
        parseQueryString(queryString);
        path = path.substr(0, queryPos);
    }

}

void Request::parseHeaders(const std::string& rawRequest) {
    std::stringstream requestStream(rawRequest);
    std::string line;

    // Skip the request line (the first line)
    if (!std::getline(requestStream, line)) {
        throw InvalidHttpRequestException("Request line is missing");
    }

    // Parse headers
    while (std::getline(requestStream, line) && !line.empty() && line != "\r") {
        // Remove the carriage return character if it's there
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            throw InvalidHttpRequestException("Malformed header line: " + line);
        }

        std::string headerName = line.substr(0, colonPos);
        std::string headerValue = line.substr(colonPos + 1);

        // Remove leading and trailing whitespace from the header value
        headerValue.erase(0, headerValue.find_first_not_of(" \t"));
        headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

        headers[headerName] = headerValue;
    }
}

void Request::parseQueryString(const std::string& queryString) {
    std::stringstream ss(queryString);
    std::string item;
    while (std::getline(ss, item, '&')) {
        size_t delimiterPos = item.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = item.substr(0, delimiterPos);
            std::string value = item.substr(delimiterPos + 1);
            queryParameters[key] = value;
        } else {
            queryParameters[item] = "";
        }
    }
}

std::string Request::getMethod() const {
    return method;
}

std::string Request::getPath() const {
    return path;
}

std::string Request::getHttpVersion() const {
    return httpVersion;
}

std::map<std::string, std::string> Request::getHeaders() const {
    return headers;
}

std::string Request::getBody() const {
    return body;
}

bool fileExistsAndAccessible(const std::string& filename, int mode) {
    return (access(filename.c_str(), mode) == 0);
}

std::string determineContentType(const std::string &filePath) {
    size_t dotPosition = filePath.find_last_of(".");
    if (dotPosition != std::string::npos) {
        std::string extension = filePath.substr(dotPosition + 1);
        if (extension == "html" || extension == "htm") {
            return "text/html";
        } else if (extension == "jpg" || extension == "jpeg") {
            return "image/jpeg";
        } else if (extension == "png") {
            return "image/png";
        } else if (extension == "gif") {
            return "image/gif";
        } else if (extension == "css") {
            return "text/css";
        } else if (extension == "js") {
            return "application/javascript";
        } else if (extension == "txt") {
			return "text/plain";
		} else if (extension == "pdf") {
			return "application/pdf";
		}
    }
    return "application/octet-stream";
}

Location* Request::checkLocation(Server &server) const {
    std::vector<Location> locations = server.getLocations();

    for (size_t i = 0; i < locations.size(); i++) {
        // std::cout << "Request path : " << path << ", Location path : " << locations[i].getPath() << std::endl;
	    if (path.find(locations[i].getPath()) == 0) {
            // std::cout << "Location found: " << locations[i].getPath() << std::endl;
           if (path.size() == locations[i].getPath().size() || path[locations[i].getPath().size()] == '/') {
                return new Location(locations[i]);
            }
        }
    }
    return NULL;
}

std::string Request::getFilePath(Location* location, Server server) const {
    if (location) {
        if (!location->getRoot().empty()) {
            return location->getRoot() + path;
        } else {
            return server.getRoot() + path;
        }
    } else {
        return server.getRoot() + path;
    }
}

bool containsString(const std::vector<std::string>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

bool Request::checkMethod(Location *location, Server &server, const std::string& methodToCheck) const {
	return (method == methodToCheck && (location != NULL ? (location->getAllow().size() == 0 ? containsString(server.getAllow(), methodToCheck) : containsString(location->getAllow(), methodToCheck)) : containsString(server.getAllow(), methodToCheck)));
}

std::string Request::generateResponse(Server &server) const {
    Response response(200, "OK");
    Location* location = checkLocation(server);
    std::string filePath = getFilePath(location, server);

	if (shouldRedirect(location, server)) {
		handleRedirect(location, server, response);
	} else if (checkMethod(location, server, "GET")) {
        handleGetRequest(server, response, location, filePath);
    } else if (checkMethod(location, server, "POST")) {
        handlePostRequest(server, response, location, filePath);
    } else if (checkMethod(location, server, "DELETE")) {
        handleDeleteRequest(server, response, filePath);
    } else {
        handleUnsupportedMethod(server, response);
    }
    if (location)
        delete(location);
    return response.generateResponse();
}

bool isDirectory(const std::string &path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        return s.st_mode & S_IFDIR;
    }
    return false;
}

std::string Request::generateDirectoryListingHTML(const std::string &directoryPath) const {
    std::ostringstream oss;
    oss << "<html><body><h1>Directory listing for " << directoryPath << "</h1><ul>";

    DIR *dir = opendir(directoryPath.c_str());
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name != "." && name != "..") {
                oss << "<li><a href=\"" << path + (path[path.size() -1] != '/' ? "/" : "") + name << "\">" << name << "</a></li>";
            }
        }
        closedir(dir);
    }
    oss << "</ul></body></html>";
    return oss.str();
}

bool checkIndexExistence(Location *location, Server &server) {
    return (location != NULL ? (location->getIndex().empty() ? (server.getIndex().empty() ? false : true) : true) : (server.getIndex().empty() ? false : true));
}

std::string getIndex(Location *location, Server &server) {
    return (location != NULL ? (location->getIndex().empty() ? server.getIndex() : location->getIndex()) : server.getIndex());
}

bool checkAutoindexStatus(Location *location, Server &server) {
    return (location != NULL ? (location->getAutoindex() == 2 ? server.getAutoindex() : location->getAutoindex()) : server.getAutoindex());
}

bool endsWithAny(const std::string& str, const std::vector<std::string>& suffixes) {
    for (size_t i = 0; i < suffixes.size(); i++) {
        if (str.size() >= suffixes[i].size() && str.rfind(suffixes[i]) == (str.size() - suffixes[i].size())) {
            return true;
        }
    }
    return false;
}

bool checkCgiMatch(Location *location, Server &server, std::string filePath) {
    return (location != NULL ? (location->getCgiExtension().size() == 0 ? (server.getCgiExtension().size() == 0 ? false : endsWithAny(filePath, server.getCgiExtension())) : endsWithAny(filePath, location->getCgiExtension())) : (server.getCgiExtension().size() == 0 ? false : endsWithAny(filePath, server.getCgiExtension())));
}

std::vector<std::string> getCgiExtension(Location *location, Server &server) {
    return (location != NULL ? (location->getCgiExtension().size() == 0 ? server.getCgiExtension() : location->getCgiExtension()) : server.getCgiExtension());
}

void Request::handleGetRequest(Server &server, Response &response, Location *location, std::string filePath) const {

    // std::cout << "complete file path : " << filePath << std::endl;

    if (isDirectory(filePath)) {
        if (!fileExistsAndAccessible(filePath, R_OK)) {
          	response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
            return;
        }
        if (checkIndexExistence(location, server)) {
            std::string tmpfilePath = filePath + (filePath[filePath.size() - 1] != '/' ? "/" : "") + getIndex(location, server);
            if (checkAutoindexStatus(location, server) && (!fileExistsAndAccessible(tmpfilePath, F_OK) || !fileExistsAndAccessible(tmpfilePath, R_OK))) {
                std::string directoryListingHTML = generateDirectoryListingHTML(filePath);
                response.setBodyFromString(directoryListingHTML);
                response.setHeader("Content-Type", "text/html");
                return;
            }
            filePath = tmpfilePath;
        } else if (checkAutoindexStatus(location, server)) {
            std::string directoryListingHTML = generateDirectoryListingHTML(filePath);
            response.setBodyFromString(directoryListingHTML);
            response.setHeader("Content-Type", "text/html");
            return;
        } else {
			response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
            return;
        }
    }

    if (!fileExistsAndAccessible(filePath, F_OK)) {
		response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
    } else if (!fileExistsAndAccessible(filePath, R_OK)) {
		response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
    } else if (isFileEmpty(filePath)) { // Add this check
        response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
    } else if (checkCgiMatch(location, server, filePath)) {
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
	} else {
		std::string contentType = determineContentType(filePath);
		std::cout << "Content-Type of a normal get: " << contentType << std::endl;
		response.setHeader("Content-Type", contentType);
		std::ifstream file(filePath.c_str(), std::ios::binary);
		std::ostringstream buffer;
		buffer << file.rdbuf();
		response.setBodyFromString(buffer.str());
	}
}

void Request::handlePostRequest(Server &server, Response &response, Location *location, std::string filePath) const {

  if (isDirectory(filePath)) {
        if (!fileExistsAndAccessible(filePath, R_OK)) {
          	response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
            return;
        }
      //Da implementare
  }

  if (checkCgiMatch(location, server, filePath)) {
		std::vector<std::string> extensions = getCgiExtension(location, server);

		if (!fileExistsAndAccessible(filePath, F_OK)) {
			response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
			return;
		} else if (!fileExistsAndAccessible(filePath, R_OK)) {
			response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
			return;
		} else if (!fileExistsAndAccessible(filePath, X_OK)) {
			response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
			return;
		} else if (isFileEmpty(filePath)) { // Add this check
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
			return;
		}
		else {
			try
			{
				Cgi cgiHandler(filePath, extensions, *this);
				cgiHandler.prepareEnvVars(*this);
				cgiHandler.execute(response, server, *this);
			}
			catch (const std::exception &e) {
				response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
			}
		}
		return;
	}

    // Determine the target directory for uploaded files
    // std::string uploadDir = (location && !location->getUploadDir().empty()) ? location->getUploadDir() : server.getUploadDir();
    std::string uploadDir = "";

    // Check if the directory exists, create it if not
    if (!fileExistsAndAccessible(uploadDir, F_OK)) {
        if (mkdir(uploadDir.c_str(), 0755) != 0) {
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
            return;
        }
    }

    // Determine the file path for the uploaded data
    std::string filePathOut = uploadDir + "/uploaded_data.txt";

    // Check if the file is writable or does not exist (meaning we can create it)
    if (fileExistsAndAccessible(filePathOut, W_OK) || !fileExistsAndAccessible(filePathOut, F_OK)) {
        std::ofstream outFile(filePathOut.c_str());
        if (outFile) {
            outFile << body; // Write the request body to the file
            outFile.close();
            response.setStatusCode(201);
            response.setStatusMessage("Created");
            response.setBodyFromString("<html><body><h1>201 Created</h1></body></html>");
        } else {
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
        }
    } else {
		response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
    }
}

void Request::handleDeleteRequest(Server &server, Response &response, std::string filePath) const {
	// std::cout << "Delete request for file: " << filePath << std::endl;
	if(isDirectory(filePath)) {
		response.setResponseError(response, server, 405, "Method Not Allowed", server.getErrorPage405());
		return;
	}
	if (!fileExistsAndAccessible(filePath, F_OK)) {
		response.setResponseError(response, server, 404, "Not Found", server.getErrorPage404());
	} else if (!fileExistsAndAccessible(filePath, W_OK)) {
		response.setResponseError(response, server, 403, "Forbidden", server.getErrorPage403());
	} else {
		if (remove(filePath.c_str()) == 0) {
			//this is the expected response for a successful DELETE request
			response.setResponseCode(204);
			response.setBodyFromString("");
		} else {
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
		}
	}
}

void Request::handleUnsupportedMethod(Server &server, Response &response) const {
	response.setResponseError(response, server, 405, "Method Not Allowed", server.getErrorPage405());
}


std::map<std::string, std::string> Request::getQueryParameters() const {
	return queryParameters;
}

void Request::setBody(std::string body) {
    this->body = body;
}

void Request::printHeaders(const std::map<std::string, std::string > &headers ) const
{
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
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
	(void)server;
    // Check if location is not null and has a redirection rule
	std::cout << "in shouldRedirect" << std::endl;
    if (location && !location->getReturnMap().empty()) {
        return true;
    }
    return false;
}

void Request::handleRedirect(Location* location, Server& server, Response& response) const {
		(void)server;
		std::cout << "Redirecting" << std::endl;
		std::map <int, std::string> returnedMAP = location->getReturnMap();
		std::map<int, std::string>::iterator it = returnedMAP.begin();
		std::string method = getMethod();
		int statusCode = it->first;
		std::string url = it->second;
		std::cout << "Method is: " << method << std::endl;
		std::cout << "Redirecting code: " << statusCode  << std::endl;
		std::cout << "Redirecting to: " << url << std::endl;
		if (statusCode != 301 && statusCode != 302 && statusCode != 303 && statusCode != 307 && statusCode != 308) {
			return response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
		}
		if (statusCode == 301)
		{
			response.setStatusCode(statusCode);
			response.setStatusMessage("Moved Permanently");
			response.setHeader("Location", url);
			// Set additional headers
			response.setHeader("Cache-Control", "no-cache, no-store, must-revalidate"); // Set Cache-Control header
			response.setHeader("Pragma", "no-cache"); // Set Pragma header
			response.setHeader("Expires", "0"); // Set Expires header
		}
		else if (statusCode == 302)
		{
			response.setStatusCode(statusCode);
			response.setStatusMessage("Found, (Moved Temporarily)");
			response.setHeader("Location", url);
		}
		else if (statusCode == 303)
		{
			response.setStatusCode(statusCode);
			response.setStatusMessage("See Other");
			response.setHeader("Location", url);
		}
		else if (statusCode == 307)
		{
				response.setStatusCode(statusCode);
				response.setStatusMessage("Temporary Redirect");
				response.setHeader("Location", url);
		}
		else if (statusCode == 308)
		{
				std::cout << "method is " << method << std::endl;
				response.setStatusCode(statusCode);
				response.setStatusMessage("Permanent Redirect");
				response.setHeader("Location ", url);
				// Set additional headers
				// response.setHeader("Cache-Control", "no-cache, no-store, must-revalidate"); // Set Cache-Control header
				// response.setHeader("Pragma", "no-cache"); // Set Pragma header
				// response.setHeader("Expires", "0"); // Set Expires header
		}
		else
		{
			std::cout << "Error in redirection" << std::endl;
			response.setResponseError(response, server, 500, "Internal Server Error", server.getErrorPage500());
		}

		return;
}
