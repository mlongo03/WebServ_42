#include "Response.hpp"
#include <sstream>   // For std::ostringstream
#include <cstdlib>   // For std::atoi

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

    // C++98 doesn't have std::to_string, so we use a stringstream instead
    std::ostringstream oss;
    oss << body.size();
    headers["Content-Length"] = oss.str();
}

std::string Response::toString() const {
    std::ostringstream responseStream;

    responseStream << "HTTP/1.1 " << statusCode << " OK\r\n";

    // Iterate through the map and add headers to the response stream
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        responseStream << it->first << ": " << it->second << "\r\n";
    }

    responseStream << "\r\n" << body;

    return responseStream.str();
}

