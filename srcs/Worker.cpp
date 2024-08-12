#include "Worker.hpp"
#include "SignalHandler.hpp"
#define BACKLOG 10


Worker::Worker(const std::vector<Server>& servers) {
    this->running = 1;
    this->servers = servers;
	for (size_t i = 0; i < servers.size(); i++) {
		try
		{
			createSocket(servers[i]);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
    }
    SignalHandler::getInstance().setupSignalHandlers(&running);
}

Worker::~Worker() {
    closeSockets();
}

void	Worker::closeSockets()
{
    for (size_t i = 0; i < listeningSockets.size(); i++) {
        close(listeningSockets[i]);
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

void Worker::createSocket(const Server& server) {
    int sockfd;
    struct addrinfo hints, *res, *p;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server.getHost().c_str(), server.getListen().c_str(), &hints, &res) != 0) {
        throw std::runtime_error("getaddrinfo error");
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            close(sockfd);
            throw std::runtime_error("setsockopt error");
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if (p == NULL) {
        std::string error = "\033[1;31mfailed to bind socket\nHost : " + server.getHost() + "\nPort : " + server.getListen() + "\033[0m";
        throw std::runtime_error(error);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        close(sockfd);
        throw std::runtime_error("listen error");
    }

    listeningSockets.push_back(sockfd);
    epollHandler.addFd(sockfd, EPOLLIN | EPOLLOUT);

	// server.print();
	std::cout << server << std::endl;
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

void Worker::assignServerToClient(const Request& request, std::vector<Client>::iterator it) {
    std::map<std::string, std::string> headers = request.getHeaders();
    std::string host = headers.count("Host") ? headers.at("Host") : "";
    std::string clientIP = it->getHost();
    std::string clientPort = it->getPort();

    std::vector<Server> filteredServers;
    for (size_t i = 0; i < servers.size(); i++) {
        const Server& server = servers[i];
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
        if (it != clientSockets.end()) {
            it->setServer(selectedServer);
        }
    } else {
        selectedServer = &filteredServers.front();
    }
}

void Worker::handleClientData(int clientSocket) {
    char buffer[1024];
    int bytesRead;
    std::string request;
    std::vector<Client>::iterator it = clientSockets.begin();

    while ((bytesRead = recv(clientSocket, buffer, sizeof buffer, 0)) > 0) {
        request.append(buffer, bytesRead);

        if (isCompleteRequest(request)) {
            break;
        }
    }

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
            Request request(request);
            // Assign server based on the request
            assignServerToClient(request, it);
            std::cout << "assigned server = " << it->getServer() << std::endl;
        }
    }
}

void Worker::handleWritableData(int clientSocket) {
    (void)clientSocket;
    // Implement how you want to handle writable events
    // For example, you might have a buffer to write data to the socket
}

std::vector<int> Worker::getSockets() const {
    return this->listeningSockets;
}

std::vector<int> Worker::getListeningSockets() const {
    return listeningSockets;
}

std::vector<Client> Worker::getClientSockets() const {
    return clientSockets;
}

