#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Response {
public:
    Response(int statusCode, const std::string& statusMessage);
    void setBodyFromFile(const std::string& filePath);
    std::string generateResponse() const;

private:
    int statusCode;
    std::string statusMessage;
    std::string body;

    std::string getDefaultErrorHtml() const;
};

#endif
