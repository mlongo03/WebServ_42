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
    // std::string cgiRoot;            // The filesystem root where CGI scripts are located
    std::string script_name;
	std::string path_info;         // The path to the CGI script to be executed
    std::string method;             // HTTP method (GET, POST, etc.)
    std::string body;               // The body of the HTTP request (for POST)
	std::string query_string;	   // The query string of the URL if present
	std::vector<std::string> cgiExtensions;  // Supported CGI extensions (e.g., .pl, .py)
    std::map<std::string, std::string> envVars; // Environment variables for the CGI script

public:

	Cgi();
	Cgi(const Cgi &src);
	Cgi &operator=(const Cgi &src);
	~Cgi();

    Cgi(const std::string &cgiBinPath, const std::vector<std::string> &extensions);

	//getters
	std::string getScriptName() const;
	std::string getMethod() const;
	std::string getBody() const;
	std::string getCgiBin() const;
	std::string getPath_info() const;
	std::string getQueryString() const;
	std::map<std::string, std::string> getEnvVars() const;
	std::vector<std::string> getCgiExtensions() const;
	// std::string getCgiRoot() const;

    // Setters for method, script path, and request body
    void setMethod(const std::string &httpMethod);
    void setBody(const std::string &requestBody);
    void setScriptName(const std::string &path);
	void setPath_info (const std::string &path);
	void setQueryString(const std::string &queryString);
    bool isCgiRequest(const std::string &url);
	void prepareEnvVars(const std::string &postBody, const std::string &contentType);

    // Convert map of environment variables to char* array for execve
    char **buildEnvArray();

	void extract_query_string(const std::string &path);
	std::string handleCgiRequest(Cgi &cgi, const std::string &path, const std::string &getMethod, const std::string &postBody, const std::string &contentType, const std::string &root);
    std::string execute();
	std::string makeRelativePath(const std::string &path);
	std::string check_correct_header(std::string &result);
	std::string generateErrorResponse(int statusCode, const std::string& statusMessage, const std::string& errorMessage);
};

#endif
