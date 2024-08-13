#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Global.hpp"
#include "Server.hpp"
#include "Socket.hpp"

class Client {
public:
    Client();
    ~Client();
    Client(int fd, std::string host, std::string port, Socket socket);
    Client(int fd, std::string host, std::string port, Socket socket, Server *server);

    Client(const Client &client);
    Client &operator=(const Client &client);

    int getFd() const;
    std::string getHost() const;
    std::string getPort() const;
    Server* getServer() const;
    Socket getSocket() const;
    void setFd(int fd);
    void setHost(std::string host);
    void setPort(std::string port);
    void setServer(Server *server);
    void setSocket(Socket socket);
    bool hasServer() const;
	bool operator==(int socket);

private:
    int fd;
    std::string host;
    std::string port;
    Socket socket;
    Server *server;
};

std::ostream& operator<<(std::ostream& os, const Client& client);

#endif

