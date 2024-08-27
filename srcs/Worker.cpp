#include "Worker.hpp"
#include "SignalHandler.hpp"
#define BACKLOG 10
#define BUFFER_LENGHT 1024


Worker::Worker(const std::vector<Server>& servers) {
    this->running = 1;
    this->servers = servers;

    for (size_t i = 0; i < servers.size(); i++) {
        try {
            bool alreadyBound = false;
            for (size_t j = 0; j < listeningSockets.size(); j++) {
                if (listeningSockets[j].getIp() == hostToIp(servers[i].getHost()) &&
                    listeningSockets[j].getPort() == servers[i].getListen()) {
                    alreadyBound = true;
                    std::cout << servers[i] << std::endl;
                    break;
                }
            }

            if (!alreadyBound) {
                Socket socket = Socket(servers[i]);
                listeningSockets.push_back(socket);
                epollHandler.addFd(socket.getSocketFd(), EPOLLIN | EPOLLOUT);
            }

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    int efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        return;
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
                    std::vector<Socket>::iterator socket = std::find(listeningSockets.begin(), listeningSockets.end(), events[i].data.fd);
                    if (socket != listeningSockets.end()) {
                        handleNewConnection(*socket);
                    } else {
                        std::vector<Client>::iterator client = std::find(clientSockets.begin(), clientSockets.end(), events[i].data.fd);
                        if (client != clientSockets.end())
                            handleClientData(*client);
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

void Worker::handleNewConnection(Socket &socket) {
    struct sockaddr_storage clientAddr;
    socklen_t addrSize = sizeof clientAddr;
    char clientIP[INET6_ADDRSTRLEN];
    int clientPort;

    int clientSocket = accept(socket.getSocketFd(), (struct sockaddr*)&clientAddr, &addrSize);

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
    Client client = Client(clientSocket, clientIP, oss.str(), socket);
    clientSockets.push_back(client);
    std::cout << "Accepted new connection on socket " << socket.getSocketFd() << ", created tcp connection with fd " << clientSocket << std::endl;
}

bool Worker::isCompleteRequest(const std::string& request) {
    // For HTTP, you might check for the end of the headers (double CRLF \r\n\r\n)
    return request.find("\r\n\r\n") != std::string::npos;
}

std::string Worker::hostToIp(std::string host) {

    struct addrinfo hints, *res;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return "";
    }

	void *addr;
	if (res->ai_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
		addr = &(ipv4->sin_addr);
	} else {
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
		addr = &(ipv6->sin6_addr);
	}

    inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));
    freeaddrinfo(res);
    return std::string(ipstr);

}

bool containsAny(const std::string& target, const std::vector<std::string>& words) {
    for (size_t i = 0; i < words.size(); i++) {
        if (target.find(words[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void Worker::assignServerToClient(const Request& request, Client &client) {
    std::map<std::string, std::string> headers = request.getHeaders();
    std::string host = headers.count("Host") ? headers.at("Host") : "";
    std::cout << "host : " << host << std::endl;
    Socket clientSocket = client.getSocket();

    std::vector<Server> filteredServers;
    for (size_t i = 0; i < servers.size(); i++) {
        const Server& server = servers[i];
        // std::cout << hostToIp(server.getHost()) << ":" << server.getListen() << " == " << clientSocket.getIp() << ":" << clientSocket.getPort() << std::endl;
        if (hostToIp(server.getHost()) == clientSocket.getIp() && server.getListen() == clientSocket.getPort()) {
            filteredServers.push_back(server);
        }
    }

    Server* selectedServer = NULL;
    for (size_t i = 0; i < filteredServers.size(); i++) {
        Server& server = filteredServers[i];
        const std::vector<std::string>& serverNames = server.getServerNames();
        if (containsAny(host, serverNames)) {
            selectedServer = &server;
            break;
        }
    }

    if (selectedServer != NULL) {
        client.setServer(&(*std::find(servers.begin(), servers.end(), *selectedServer)));
    } else {
        client.setServer(&(*std::find(servers.begin(), servers.end(), filteredServers.front())));
    }
    // std::cout << "assigned server = " << client.getServer()->getHost() << std::endl;
}

void Worker::handleClientData(Client &client) {
    char buffer[BUFFER_LENGHT];
    std::string request;
    int bytesRead;

    while ((bytesRead = recv(client.getFd(), buffer, sizeof buffer, 0)) > 0) {
        request.append(buffer, bytesRead);
        if (isCompleteRequest(request) || request.length() < BUFFER_LENGHT) {
            break;
        }
    }

    if (bytesRead <= 0) {
        epollHandler.removeFd(client.getFd());
        clientSockets.erase(std::find(clientSockets.begin(), clientSockets.end(), client.getFd()));
        close(client.getFd());
        std::cout << "Connection closed on socket " << client.getFd() << std::endl;
    } else {
        // Handle HTTP request and send response
        std::cout << "message got = " << request << std::endl;
        if (!client.hasServer()) {
            // Create Request object
            Request request1 = Request(request);
            assignServerToClient(request1, client);
			// executeCgi(request1, client);
            // std::cout << "assigned server = " << *client.getServer() << std::endl;
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

