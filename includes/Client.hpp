#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Global.hpp"
#include "Server.hpp"

class Client {
public:
    Client();
    ~Client();
    Client(int fd, std::string host, std::string port);
    Client(int fd, std::string host, std::string port, Server *server);

    Client(const Client &client);
    Client &operator=(const Client &client);

    int getFd() const;
    std::string getHost() const;
    std::string getPort() const;
    Server* getServer() const;
    void setFd(int fd);
    void setHost(std::string host);
    void setPort(std::string port);
    void setServer(Server *server);
    bool hasServer() const;

private:
    int fd;
    std::string host;
    std::string port;
    Server *server;
};

std::ostream& operator<<(std::ostream& os, const Client& client);

#endif

