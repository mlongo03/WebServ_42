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
#include <ctime>
#include "Exception.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "Cgi.hpp"
#include "Utils.hpp"

class Request {
	private:
		std::string method;
		std::string path;
		std::string httpVersion;
		std::map<std::string, std::string> headers;
		std::string body;
		std::map<std::string, std::string> queryParameters;
        time_t start_time;

	public:
		Request(const std::string& rawRequest);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getHttpVersion() const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
		void setBody(std::string body);
		std::string generateResponse(Server &server) const;
		std::map<std::string, std::string> getQueryParameters() const;

		std::string getFileExtensionFromContentType() const;
		void parseRequest(const std::string& rawRequest);
		void parseHeaders(const std::string& rawRequest);
		void handleGetRequest(Server &server, Response &response, Location *location, std::string filePath) const;
		void handlePostRequest(Server &server, Response &response, Location *location, std::string filePath) const;
		void handleDeleteRequest(Server &server, Response &response, std::string filePath) const;
		void handleUnsupportedMethod(Server &server, Response &response) const;
		std::string generateDirectoryListingHTML(const std::string &directoryPath) const;
		Location* checkLocation(Server &server) const;
		std::string getFilePath(Location* location, Server server) const;
		void parseQueryString(const std::string& queryString); // New method to parse query strings
		bool checkMethod(Location *location, Server &server, const std::string& methodToCheck) const;
		void printHeaders(const std::map<std::string, std::string > &headers ) const;
		std::string getContentType() const;
        time_t getStartTime() const;
		std::string getFilenameFromPath(const std::string& path) const;
		std::string generateUniqueFilename() const;
		bool shouldRedirect(Location* location, Server& server) const;
		void handleRedirect(Location* location, Server& server, Response& response) const;
		void setRedirectResponse(Response& response, int statusCode,const std::string& statusMessage, std::string& url) const;
};

#endif
