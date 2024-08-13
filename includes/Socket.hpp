#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "Global.hpp"
#include "Server.hpp"

class Socket {
public:
    // Constructors and Destructor
    Socket();
    Socket(const Server& server);
    ~Socket();

    // Getters
    int getSocketFd() const;
    std::string getPort() const;
    std::string getIp() const;
    std::string getHost() const;

    // Setters
    void setSocketFd(int fd);
    void setPort(const std::string &port);
    void setIp(const std::string &ip);
    void setHost(const std::string &host);
	bool operator==(int socket);

private:
    int socketfd;
    std::string port;
    std::string ip;
    std::string host;
};

#endif
