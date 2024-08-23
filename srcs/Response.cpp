#include "Response.hpp"

Response::Response(int statusCode, const std::string& statusMessage)
    : statusCode(statusCode), statusMessage(statusMessage) {}

void Response::setBodyFromFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        body = buffer.str();
    } else {
        body = getDefaultErrorHtml();
    }
}

std::string Response::generateResponse() const {
    std::ostringstream responseStream;

    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    responseStream << "Content-Length: " << body.size() << "\r\n";
    responseStream << "Content-Type: text/html\r\n";
    responseStream << "\r\n";

    responseStream << body;

    return responseStream.str();
}

std::string Response::getDefaultErrorHtml() const {
    std::ostringstream html;

    html << "<html><head><title>" << statusCode << " " << statusMessage << "</title></head>"
         << "<body><h1>" << statusCode << " " << statusMessage << "</h1>"
         << "<p>Sorry, something went wrong.</p></body></html>";

    return html.str();
}

void Response::setStatusCode(int status) {
    this->statusCode = status;
}

void Response::setStatusMessage(std::string statusMessage) {
    this->statusMessage = statusMessage;
}

void Response::setBodyFromString(std::string body) {
    this->body = body;
}
