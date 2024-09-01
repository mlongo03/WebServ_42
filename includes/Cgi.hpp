#ifndef CGI_HPP
#define CGI_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>    // For environment manipulation
#include <unistd.h>   // For fork, execve, pipe
#include <sys/wait.h> // For waitpid
#include <fcntl.h>    // For file control operations
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <sstream>
#include "Utils.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"

enum RequestType {
    GET_REQUEST,
    POST_REQUEST
};

class Server;
class Request;
class Response;

class Cgi {
	private:
		std::string path_info;         // The path to the CGI script to be executed
		std::string script_name;
		std::string method;             // HTTP method (GET, POST, etc.)
		std::string body;               // The body of the HTTP request (for POST)
		std::vector<std::string> cgiExtensions;
		std::map<std::string, std::string> envVars;
		std::map<std::string, std::string> queryParameters; // Query parameters extracted from the URL

	public:

		Cgi();
		Cgi(const Cgi &src);
		Cgi &operator=(const Cgi &src);
		~Cgi();

		Cgi(const std::string &path, const std::vector<std::string> &extensions,const Request &request);

		//getters
		std::string getScriptName() const;
		std::string getMethod() const;
		std::string getBody() const;
		std::string getPath_info() const;
		std::map<std::string, std::string> getEnvVars() const;
		std::vector<std::string> getCgiExtensions() const;
		std::map<std::string, std::string> getQueryParameters() const;

		// Setters for method, script path, and request body
		void setMethod(const std::string &httpMethod);
		void setBody(const std::string &requestBody);
		void setScriptName(const std::string &path);
		void setPath_info (const std::string &path);
		void prepareEnvVars(const Request &request);
		void setQueryParameters(const std::map<std::string, std::string> &params);

		// Convert map of environment variables to char* array for execve
		char **buildEnvArray();

		void extract_script_name(const std::string &path);
		// std::string handleCgiRequest(Cgi &cgi, const std::string &getMethod, const std::string &postBody, const std::string &contentType);
		void	execute(Response &response, Server &server, const Request &request);
		bool check_correct_header(std::string &result, Response &response, Server &server,const Request &request);
		std::string getBodyFromResponse(const std::string& response);
};

#endif
