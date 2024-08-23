#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

class Response {
private:
    int statusCode;
    std::string statusMessage;
    std::string body;
    std::map<std::string, std::string> headers;

public:
    Response(int statusCode, const std::string& statusMessage);

    void setStatusCode(int code);
    void setStatusMessage(const std::string &message);
    void setBodyFromString(const std::string &bodyContent);
    void setBodyFromFile(const std::string &filePath);
    void setHeader(const std::string &key, const std::string &value);
    void setContentLength(size_t length);

    std::string generateResponse() const;
    void setDefaultErrorBody();
};

#endif
