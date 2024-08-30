#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include "Exception.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "Cgi.hpp"

class Request {
public:
    Request(const std::string& rawRequest);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getHttpVersion() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;
    std::string generateResponse(Server &server) const;
    std::map<std::string, std::string> getQueryParameters() const;

private:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> queryParameters;

    void parseRequest(const std::string& rawRequest);
    void parseHeaders(const std::string& headersPart);
    void handleGetRequest(Server &server, Response &response, Location *location, std::string filePath) const;
    void handlePostRequest(Server &server, Response &response, Location *location, std::string filePath) const;
    void handleDeleteRequest(Server &server, Response &response, Location *location, std::string filePath) const;
    void handleUnsupportedMethod(Server &server, Response &response) const;
    std::string generateDirectoryListingHTML(const std::string &directoryPath) const;
    Location* checkLocation(Server &server) const;
    std::string getFilePath(Location* location, Server server) const;
    void parseQueryString(const std::string& queryString); // New method to parse query strings
    bool checkMethod(Location *location, Server &server, const std::string& methodToCheck) const;
	void printHeaders(const std::map<std::string, std::string > &headers ) const;
	std::string getContentType() const;
};

#endif
