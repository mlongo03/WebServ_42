#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Global.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "Response.hpp"

class Client {
public:
    Client();
    ~Client();
    Client(int fd, std::string host, std::string port, Socket socket);

    Client(const Client &client);
    Client &operator=(const Client &client);

    int getFd() const;
    std::string getHost() const;
    std::string getPort() const;
    Server* getServer() const;
    Socket getSocket() const;
    std::string getRequest() const;
    std::string getResponse() const;
    void setFd(int fd);
    void setHost(std::string host);
    void setPort(std::string port);
    void setServer(Server *server);
    void setSocket(Socket socket);
    void setRequest(std::string request);
    void setResponse(std::string response);
    bool hasServer() const;
	bool operator==(int socket);

private:
    int fd;
    std::string host;
    std::string port;
    Socket socket;
    std::string request;
    std::string response;
    Server *server;
	// std>>string request;
	// std::string response;
};

std::ostream& operator<<(std::ostream& os, const Client& client);

#endif

