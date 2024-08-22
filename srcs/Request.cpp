#include "Request.hpp"
#include <sstream>
#include <algorithm>
#include <vector>
#include <iostream>

Request::Request(const std::string& rawRequest) {
    parseRequest(rawRequest);
}

std::vector<std::string> splitBySpaces(const std::string& str) {
    std::stringstream ss(str);
    std::string word;
    std::vector<std::string> result;
    while (ss >> word) {
        result.push_back(word);
    }
    return result;
}

void Request::parseRequest(const std::string& rawRequest) {
    std::stringstream requestStream(rawRequest);
    std::string requestLine;

    // Get the first line (the Request Line)
    if (!std::getline(requestStream, requestLine)) {
        throw InvalidHttpRequestException("Request line is missing");
    }

    // Split the request line into method, URI, and HTTP version
    std::vector<std::string> parts = splitBySpaces(requestLine);

    if (parts.size() != 3) {
        throw InvalidHttpRequestException("Request line must have a method, URI, and HTTP version");
    }

    method = parts[0];
    path = parts[1];
    httpVersion = parts[2];

    std::cout << "method = " << method << ", path = " << path << ", http version = " << httpVersion << std::endl;

    // Validate method
    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw InvalidHttpRequestException("Unsupported HTTP method: " + method);
    }

    // Validate URI
    if (path.empty() || path[0] != '/') {
        throw InvalidHttpRequestException("Invalid or missing URI: " + path);
    }

    // Validate HTTP version
    if (httpVersion != "HTTP/1.1") {
        throw InvalidHttpRequestException("Unsupported HTTP version: " + httpVersion);
    }

    // Parse headers
    std::string headerLine;
    while (std::getline(requestStream, headerLine) && headerLine != "\r") {
        size_t delimiterPos = headerLine.find(":");
        if (delimiterPos == std::string::npos) {
            throw InvalidHttpRequestException("Malformed header: " + headerLine);
        }
        std::string headerName = headerLine.substr(0, delimiterPos);
        std::string headerValue = headerLine.substr(delimiterPos + 1);
        headers[headerName] = headerValue;
    }

    // Parse body (if present)
    body = std::string(std::istreambuf_iterator<char>(requestStream), std::istreambuf_iterator<char>());
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
