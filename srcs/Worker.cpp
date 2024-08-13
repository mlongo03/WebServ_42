#include "Worker.hpp"
#include "SignalHandler.hpp"
#define BACKLOG 10


Worker::Worker(const std::vector<Server>& servers) {
    this->running = 1;
    this->servers = servers;
	for (size_t i = 0; i < servers.size(); i++) {
		try
		{
            Socket socket = Socket(servers[i]);
			listeningSockets.push_back(socket);
            epollHandler.addFd(socket.getSocketFd(), EPOLLIN | EPOLLOUT);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
    }
    int efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        return ;
    }
    epollHandler.addFd(efd, EPOLLIN | EPOLLOUT);
    SignalHandler::getInstance().setupSignalHandlers(&running, efd);
}

Worker::~Worker() {
    closeSockets();
}

void	Worker::closeSockets()
{
    for (size_t i = 0; i < listeningSockets.size(); i++) {
        close(listeningSockets[i].getSocketFd());
    }
    for (size_t i = 0; i < clientSockets.size(); i++) {
        close(clientSockets[i].getFd());
    }
}

void Worker::run()
{
    if (listeningSockets.size() == 0)
        return ;
    while (running) {
        try
        {
            std::vector<struct epoll_event> events = epollHandler.waitForEvents();
            if (!running)
                break ;
            for (size_t i = 0; i < events.size(); i++) {
                if (events[i].events & EPOLLIN) {
                    if (std::find(listeningSockets.begin(), listeningSockets.end(), events[i].data.fd) != listeningSockets.end()) {
                        handleNewConnection(events[i].data.fd);
                    } else {
                        handleClientData(events[i].data.fd);
                    }
                }
                if (events[i].events & EPOLLOUT) {
                    handleWritableData(events[i].data.fd);
                }
            }
        }
        catch(const std::exception& e)
        {
			std::cerr << e.what() << std::endl;
			closeSockets();
			return ;
        }
    }
}

void Worker::handleNewConnection(int listeningSocket) {
    struct sockaddr_storage clientAddr;
    socklen_t addrSize = sizeof clientAddr;
    char clientIP[INET6_ADDRSTRLEN];
    int clientPort;

    int clientSocket = accept(listeningSocket, (struct sockaddr*)&clientAddr, &addrSize);

    if (clientSocket == -1) {
        std::cerr << "accept error" << std::endl;
        return;
    }

    epollHandler.addFd(clientSocket, EPOLLIN);
    if (clientAddr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&clientAddr;
        inet_ntop(AF_INET, &s->sin_addr, clientIP, sizeof clientIP);
        clientPort = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&clientAddr;
        inet_ntop(AF_INET6, &s->sin6_addr, clientIP, sizeof clientIP);
        clientPort = ntohs(s->sin6_port);
    }
    std::ostringstream oss;
    oss << clientPort;
    oss.str();
    std::cout << "Creating Client object with fd = " << clientSocket << ", socket = " << clientIP << ", port = " << oss.str() << std::endl;
    Client client = Client(clientSocket, clientIP, oss.str());
    clientSockets.push_back(client);
    std::cout << "Accepted new connection on socket " << listeningSocket << ", created tcp connection with fd " << clientSocket << std::endl;
}

bool Worker::isCompleteRequest(const std::string& request) {
    // For HTTP, you might check for the end of the headers (double CRLF \r\n\r\n)
    return request.find("\r\n\r\n") != std::string::npos;
}

void Worker::assignServerToClient(const Request& request, std::vector<Client>::iterator *it) {
    std::map<std::string, std::string> headers = request.getHeaders();
    std::string host = headers.count("Host") ? headers.at("Host") : "";
    std::string clientIP = (*it)->getHost();
    std::string clientPort = (*it)->getPort();

    std::vector<Server> filteredServers;
    for (size_t i = 0; i < servers.size(); i++) {
        const Server& server = servers[i];
        // std::cout << server.getHost() << ":" << server.getListen() << " == " << clientIP << ":" << clientPort << std::endl;
        if (server.getHost() == clientIP && server.getListen() == clientPort) {
            filteredServers.push_back(server);
        }
    }

    Server* selectedServer = NULL;
    for (size_t i = 0; i < filteredServers.size(); i++) {
        const Server& server = filteredServers[i];
        const std::vector<std::string>& serverNames = server.getServerNames();
        if (std::find(serverNames.begin(), serverNames.end(), host) != serverNames.end()) {
            selectedServer = const_cast<Server*>(&server);
            break;
        }
    }

    if (selectedServer != NULL) {
        if (*it != clientSockets.end()) {
            (*it)->setServer(selectedServer);
        }
    } else {
        (*it)->setServer(&filteredServers.front());
    }
}

void Worker::handleClientData(int clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof buffer, 0);
    std::string request(buffer, bytesRead);
    std::vector<Client>::iterator it = clientSockets.begin();

    // while ((bytesRead = recv(clientSocket, buffer, sizeof buffer, 0)) > 0) {
    //     request.append(buffer, bytesRead);

    //     std::cout << "test" << request << std::endl;
    //     if (isCompleteRequest(request)) {
    //         break;
    //     }
    // }

    for (; it->getFd() != clientSocket; it++) {
    }

    if (bytesRead <= 0) {
        epollHandler.removeFd(clientSocket);
        clientSockets.erase(it);
        close(clientSocket);
        std::cout << "Connection closed on socket " << clientSocket << std::endl;
    } else {
        // Handle HTTP request and send response
        std::cout << "message got = " << request << std::endl;
        if (!it->hasServer()) {
            // Create Request object
            Request request1 = Request(request);
            // Assign server based on the request
            assignServerToClient(request1, &it);
            // std::cout << "assigned server = " << it->getServer() << std::endl;
        }
    }
}

void Worker::handleWritableData(int clientSocket) {
    (void)clientSocket;
    // Implement how you want to handle writable events
    // For example, you might have a buffer to write data to the socket
}

std::vector<Socket> Worker::getListeningSockets() const {
    return listeningSockets;
}

std::vector<Client> Worker::getClientSockets() const {
    return clientSockets;
}

