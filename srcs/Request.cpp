#include "Request.hpp"
#include <sstream>
#include <algorithm>

Request::Request(const std::string& rawRequest) {
    parseRequest(rawRequest);
}

void Request::parseRequest(const std::string& rawRequest) {
    std::istringstream stream(rawRequest);
    std::string line;

    std::getline(stream, line);
    std::istringstream requestLineStream(line);
    requestLineStream >> method >> path >> httpVersion;

    // Headers
    std::string headersPart;
    while (std::getline(stream, line) && line != "\r") {
        headersPart += line + "\n";
    }
    parseHeaders(headersPart);

    // Body
    body = std::string((std::istreambuf_iterator<char>(stream)),
                        std::istreambuf_iterator<char>());
}

void Request::parseHeaders(const std::string& headersPart) {
    std::istringstream headersStream(headersPart);
    std::string headerLine;
    while (std::getline(headersStream, headerLine)) {
        std::string::size_type colonPos = headerLine.find(':');
        if (colonPos != std::string::npos) {
            std::string key = headerLine.substr(0, colonPos);
            std::string value = headerLine.substr(colonPos + 1);
            value.erase(0, value.find_first_not_of(' ')); // Trim leading spaces
            headers[key] = value;
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
