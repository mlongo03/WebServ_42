#ifndef Worker_HPP
#define Worker_HPP

#include "Global.hpp"
#include "Server.hpp"
#include "Epoll.hpp"

class Worker {
public:
    Worker(const std::vector<Server>& servers);
    ~Worker();
    std::vector<int> getSockets() const;

    void run();

private:
    void createSocket(const Server& server);
    void handleNewConnection(int listeningSocket);
    void handleClientData(int clientSocket);
    void handleWritableData(int clientSocket);
    void closeSockets();

    std::vector<int> listeningSockets;
    Epoll epollHandler;
};

#endif
