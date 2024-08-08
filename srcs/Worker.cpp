#include "Worker.hpp"
#include "SignalHandler.hpp"
#define BACKLOG 10

Worker::Worker(const std::vector<Server>& servers) {
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
    SignalHandler::getInstance().setupSignalHandlers(listeningSockets, this);
}

Worker::~Worker() {
    // std::cout << "destroying woker class" << std::endl;
    for (size_t i = 0; i < listeningSockets.size(); i++) {
        close(listeningSockets[i]);
    }
}

void Worker::run() {

    if (listeningSockets.size() == 0)
        return ;

    while (true) {
        std::vector<struct epoll_event> events = epollHandler.waitForEvents();

        for (size_t i = 0; i < events.size(); i++) {
            if (std::find(listeningSockets.begin(), listeningSockets.end(), events[i].data.fd) != listeningSockets.end()) {
                handleNewConnection(events[i].data.fd);
            } else {
                handleClientData(events[i].data.fd);
            }
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

    if (getaddrinfo(server.host.c_str(), server.listen.c_str(), &hints, &res) != 0) {
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
    epollHandler.addFd(sockfd, EPOLLIN);

	server.print();
}

void Worker::handleNewConnection(int listeningSocket) {
    struct sockaddr_storage clientAddr;
    socklen_t addrSize = sizeof clientAddr;
    int clientSocket = accept(listeningSocket, (struct sockaddr*)&clientAddr, &addrSize);

    if (clientSocket == -1) {
        std::cerr << "accept error" << std::endl;
        return;
    }

    epollHandler.addFd(clientSocket, EPOLLIN);
    std::cout << "Accepted new connection on socket " << clientSocket << std::endl;
}

void Worker::handleClientData(int clientSocket) {
    char buffer[1024];
    int bytesRead = read(clientSocket, buffer, sizeof buffer);

    if (bytesRead <= 0) {
        close(clientSocket);
        epollHandler.removeFd(clientSocket);
        std::cout << "Connection closed on socket " << clientSocket << std::endl;
    } else {
        // Handle HTTP request and send response
    }
}


std::vector<int> Worker::getSockets() const {
    return this->listeningSockets;
}

