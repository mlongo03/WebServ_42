#ifndef CGI_HPP
#define CGI_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
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
		std::string path_info;
		std::string script_name;
		std::string method;
		std::string body;
		std::vector<std::string> cgiExtensions;
		std::map<std::string, std::string> envVars;
		std::map<std::string, std::string> queryParameters;

	public:

		Cgi();
		Cgi(const Cgi &src);
		Cgi &operator=(const Cgi &src);
		~Cgi();

		Cgi(const std::string &path, const std::vector<std::string> &extensions,const Request &request);

		std::string getScriptName() const;
		std::string getMethod() const;
		std::string getBody() const;
		std::string getPath_info() const;
		std::map<std::string, std::string> getEnvVars() const;
		std::vector<std::string> getCgiExtensions() const;
		std::map<std::string, std::string> getQueryParameters() const;

		void setMethod(const std::string &httpMethod);
		void setBody(const std::string &requestBody);
		void setScriptName(const std::string &path);
		void setPath_info (const std::string &path);
		void prepareEnvVars(const Request &request);
		void setQueryParameters(const std::map<std::string, std::string> &params);

		char **buildEnvArray();

		void extract_script_name(const std::string &path);
		void	execute(Response &response, Server &server, const Request &request);
		bool check_correct_header(std::string &result, Response &response, Server &server,const Request &request);
		std::string getBodyFromResponse(const std::string& response);
};

#endif
