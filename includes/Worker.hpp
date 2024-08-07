#ifndef Worker_HPP
#define Worker_HPP

#include "Global.hpp"
#include "Server.hpp"
#include "Epoll.hpp"

class Worker {
public:
    Worker(const std::vector<Server>& servers);
    ~Worker();

    void run();

private:
    void createSocket(const Server& server);
    void handleNewConnection(int listeningSocket);
    void handleClientData(int clientSocket);

    std::vector<int> listeningSockets;
    Epoll epollHandler;
};

#endif
