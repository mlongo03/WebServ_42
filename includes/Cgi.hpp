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

class Cgi {
private:
    std::string cgiBin;             // The URL path that identifies CGI requests
    std::string cgiRoot;            // The filesystem root where CGI scripts are located
    std::vector<std::string> cgiExtensions;  // Supported CGI extensions (e.g., .pl, .py)
    std::string scriptPath;         // The path to the CGI script to be executed
    std::string method;             // HTTP method (GET, POST, etc.)
    std::map<std::string, std::string> envVars; // Environment variables for the CGI script
    std::string body;               // The body of the HTTP request (for POST)

public:

	Cgi();
	Cgi(const Cgi &src);
	Cgi &operator=(const Cgi &src);
	~Cgi();

    Cgi(const std::string &cgiBinPath, const std::string &cgiRootPath, const std::vector<std::string> &extensions);

	//getters
	std::string getScriptPath() const;
	std::string getMethod() const;
	std::string getBody() const;
	std::map<std::string, std::string> getEnvVars() const;
	std::string getCgiBin() const;
	std::string getCgiRoot() const;
	std::vector<std::string> getCgiExtensions() const;
	
    // Setters for method, script path, and request body
    void setMethod(const std::string &httpMethod);
    void setScriptPath(const std::string &path);
    void setBody(const std::string &requestBody);

    // Check if the requested URL matches a CGI request
    bool isCgiRequest(const std::string &url);
    // Prepare environment variables for the CGI script
    void prepareEnvVars(const std::string &pathInfo);

    // Convert map of environment variables to char* array for execve
    char **buildEnvArray();

    std::string execute();
	std::string makeRelativePath(const std::string &path);
	std::string check_correct_header(std::string &result);
	std::string generateErrorResponse(int statusCode, const std::string& statusMessage, const std::string& errorMessage);
};

#endif
