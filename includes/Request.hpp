#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "Exception.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include <iostream>

class Request {
public:
    Request(const std::string& rawRequest);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getHttpVersion() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;
    std::string generateResponse(Server &server) const;

private:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequest(const std::string& rawRequest);
    void parseHeaders(const std::string& headersPart);
    void handleGetRequest(Server &server, Response &response) const;
    void handlePostRequest(Server &server, Response &response) const;
    void handleDeleteRequest(Server &server, Response &response) const;
    void handleUnsupportedMethod(Server &server, Response &response) const;
};

#endif
