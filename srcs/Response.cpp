#include "Response.hpp"

Response::Response(int code, const std::string &message)
    : statusCode(code), statusMessage(message) {
    setHeader("Content-Type", "text/html");
    setHeader("Connection", "close");
}

void Response::setStatusCode(int code) {
    statusCode = code;
}

void Response::setStatusMessage(const std::string &message) {
    statusMessage = message;
}

void Response::setBodyFromString(const std::string &bodyContent) {
    body = bodyContent;
    setContentLength(body.size());
}

void Response::setBodyFromFile(const std::string &filePath) {
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (file) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        body = buffer.str();
        setContentLength(body.size());
    } else {
        setDefaultErrorBody();
    }
}

void Response::setContentLength(size_t length) {
    std::stringstream ss;
    ss << length;
    setHeader("Content-Length", ss.str());
}

void Response::setHeader(const std::string &key, const std::string &value) {
    headers[key] = value;
}

std::string Response::generateResponse() const {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it) {
        responseStream << it->first << ": " << it->second << "\r\n";
    }

    responseStream << "\r\n";
    responseStream << body;
    return responseStream.str();
}

void Response::setDefaultErrorBody() {
    std::ostringstream bodyStream;
    bodyStream << "<html><head><title>" << statusCode << " " << statusMessage
               << "</title></head><body><h1>" << statusCode << " " << statusMessage
               << "</h1><p>Sorry, something went wrong.</p></body></html>";
    body = bodyStream.str();
    setContentLength(body.size());
}


void Response::setResponseError(Response& response, Server& server, int statusCode, const std::string& statusMessage, const std::string& errorPage) {
    response.setStatusCode(statusCode);
    response.setStatusMessage(statusMessage);
    response.setBodyFromFile(server.getRoot() + errorPage);
}
