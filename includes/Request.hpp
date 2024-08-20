#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request {
public:
    Request(const std::string& rawRequest);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getHttpVersion() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

private:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequest(const std::string& rawRequest);
    void parseHeaders(const std::string& headersPart);
};

#endif
