#include "Response.hpp"
#include <sstream>
#include <cstdlib>

Response::Response() : statusCode(200) {
    headers["Content-Type"] = "text/plain";
}

void Response::setStatusCode(int code) {
    statusCode = code;
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void Response::setBody(const std::string& body) {
    this->body = body;

    std::ostringstream oss;
    oss << body.size();
    headers["Content-Length"] = oss.str();
}

std::string Response::toString() const {
    std::ostringstream responseStream;

    responseStream << "HTTP/1.1 " << statusCode << " OK\r\n";

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        responseStream << it->first << ": " << it->second << "\r\n";
    }

    responseStream << "\r\n" << body;

    return responseStream.str();
}

