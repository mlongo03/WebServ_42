#ifndef Worker_HPP
#define Worker_HPP

#include "Global.hpp"
#include "Server.hpp"
#include "Epoll.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include "Exception.hpp"

class Worker {
public:
    Worker(const std::vector<Server>& servers);
    ~Worker();
    std::vector<Socket> getListeningSockets() const;
    std::vector<Client> getClientSockets() const;
    void run();

private:
    void createSocket(const Server& server);
    void handleNewConnection(Socket &socket);
    void handleClientData(Client &client);
    void handleWritableData(Client &client);
    void closeSockets();
    void assignServerToClient(const Request& request, Client &client);
    bool isCompleteRequest(const std::string& request);
    std::string hostToIp(std::string host);

    std::vector<Socket> listeningSockets;
    std::vector<Client> clientSockets;
    std::vector<Server> servers;
    Epoll epollHandler;
    int running;
};

#endif
