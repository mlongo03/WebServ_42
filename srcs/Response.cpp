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

std::string generateRandomSessionID(int length = 16) {
    std::string sessionId;
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; ++i) {
        sessionId += charset[rand() % (sizeof(charset) - 1)];
    }
    return sessionId;
}

bool isSessionCookiePresent(std::string cookies) {
    std::string sessionID = "session_id=";
    size_t pos = cookies.find(sessionID);

    // If session_id cookie is present, return the original cookies
    if (pos != std::string::npos) {
        return true;
    }

    return false;
}

void Response::setSessionCookie() {
    std::string newSessionID = generateRandomSessionID();

    setHeader("Set-Cookie", "session_id=" + newSessionID + "; Path=/; HttpOnly; Secure; Max-Age=3600");
}

std::string Response::generateResponse(std::string cookies) {
    std::ostringstream responseStream;

    // std::cout << "Cookie: " << cookies << std::endl;
    if (cookies.empty() || !isSessionCookiePresent(cookies)) {
        setSessionCookie();
    }

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

void Response::setResponseCode(int code) {
	statusCode = code;
}

void Response::printHeaders(const std::map<std::string, std::string > &headers ) const
{
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

std::map<std::string, std::string> Response::getHeaders() const {
	return headers;
}
