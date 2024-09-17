#ifndef Worker_HPP
#define Worker_HPP

#include <ctime> // For time_t and difftime
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
    void handleNewConnection(Socket &socket);
    void handleClientData(Client &client);
    void handleWritableData(Client &client);
    void closeSockets();
    void assignServerToClient(const Request& request, Client &client);
    bool isCompleteRequest(Client& client);
    void completeHeaders(std::string fullRequest, Client& client);
    void handleChunkedBody(std::string& receivedData, Client& client);
    std::string hostToIp(std::string host);
    void checkTimeouts();

    std::vector<Socket> listeningSockets;
    std::vector<Client> clientSockets;
    std::vector<Server> servers;
    Epoll epollHandler;
    int running;
};

#endif
