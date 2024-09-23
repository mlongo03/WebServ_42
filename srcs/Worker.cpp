#include "Worker.hpp"
#include "SignalHandler.hpp"
#define BACKLOG 10
#define BUFFER_LENGHT 1024


void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        throw std::runtime_error("error for fcntl");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        throw std::runtime_error("error for fcntl");
    }
}

Worker::Worker(const std::vector<Server>& servers) {
    this->running = 1;
    this->servers = servers;

    for (size_t i = 0; i < servers.size(); i++) {
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
            try
            {
                Socket socket = Socket(servers[i]);
                listeningSockets.push_back(socket);
                set_nonblocking(socket.getSocketFd());
                epollHandler.addFd(socket.getSocketFd(), EPOLLIN | EPOLLOUT);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }

        }
    }
    if (listeningSockets.size() == 0)
        throw std::runtime_error("No server can be started");

    int efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        throw std::runtime_error("Error for efd creation");
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

void Worker::checkTimeouts() {
    const int timeoutDuration = 30; // timeout duration in seconds

    time_t currentTime = std::time(NULL); // Get the current time

    for (std::vector<Client>::iterator clientIt = clientSockets.begin(); clientIt != clientSockets.end(); ) {
        if (clientIt->getRequestObject() != NULL) {
            Request* req = clientIt->getRequestObject();
            double elapsedTime = std::difftime(currentTime, req->getStartTime());
            if (elapsedTime > timeoutDuration) {
                // Timeout detected
                // std::cout << "Request timed out for client: " << clientIt->getFd() << std::endl;
                epollHandler.removeFd(clientIt->getFd());
                clientSockets.erase(clientIt);
                close(clientIt->getFd());
            } else {
                ++clientIt;
            }
        } else {
            ++clientIt;
        }
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
            checkTimeouts();
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
                    std::vector<Client>::iterator client = std::find(clientSockets.begin(), clientSockets.end(), events[i].data.fd);
                    if (client != clientSockets.end())
                        handleWritableData(*client);
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
        std::cout << "accept error" << std::endl;
        return;
    }

    set_nonblocking(clientSocket);
    epollHandler.addFd(clientSocket, EPOLLIN | EPOLLOUT);
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
    // std::cout << "Creating Client object with fd = " << clientSocket << ", socket = " << clientIP << ", port = " << oss.str() << std::endl;
    Client client = Client(clientSocket, clientIP, oss.str(), socket);
    clientSockets.push_back(client);
    // std::cout << "Accepted new connection on socket " << socket.getSocketFd() << ", created tcp connection with fd " << clientSocket << std::endl;
}

void Worker::completeHeaders(std::string fullRequest, Client& client) {
    // Find the end of the headers section
    size_t headerEnd = fullRequest.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // If we don't find the end of the headers, the request isn't complete
        // std::cout << "request not complete\n";
        return ;
    }

    // Parse and fill headers in the Request object
    // std::cout << "request complete now fill headers\n";
    if (client.getRequestObject()->getHeaders().empty()) {
        client.getRequestObject()->parseHeaders(fullRequest);
    }
}

std::string stripChunkedEncoding(const std::string& chunkedBody) {
    std::string result;
    size_t pos = 0;

    while (true) {
        // Step 1: Find the position of the next chunk size
        size_t chunkSizeEnd = chunkedBody.find("\r\n", pos);
        if (chunkSizeEnd == std::string::npos) break;

        // Step 2: Extract and parse the chunk size (in hex)
        std::string chunkSizeStr = chunkedBody.substr(pos, chunkSizeEnd - pos);
        int chunkSize = std::strtol(chunkSizeStr.c_str(), NULL, 16);

        // If chunk size is 0, we're at the last chunk
        if (chunkSize == 0) break;

        // Step 3: Move the position past the chunk size and the "\r\n"
        pos = chunkSizeEnd + 2;

        // Step 4: Append the actual chunk data to the result
        result.append(chunkedBody, pos, chunkSize);

        // Step 5: Move the position past the chunk data and the trailing "\r\n"
        pos += chunkSize + 2;
    }

    return result;
}

bool Worker::isCompleteRequest(Client& client) {

    // Find the end of the headers section
    size_t headerEnd = client.getRequest().find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // If we don't find the end of the headers, the request isn't complete
        return false;
    }

    // At this point, we have all the headers
    size_t bodyStart = headerEnd + 4; // Move past the "\r\n\r\n"

    // Check for a Content-Length header to determine if we need to wait for a body
    std::map<std::string, std::string> headers = client.getRequestObject()->getHeaders();

    if (headers.find("Content-Length") != headers.end()) {
        // Extract the Content-Length value
        int contentLength = atoi(headers["Content-Length"].c_str());


        // Check if we have received all the body data
        if (client.getRequest().size() >= bodyStart + contentLength) {
            // If body is complete, set it in the Request object
            client.getRequestObject()->setBody(client.getRequest().substr(bodyStart, contentLength));
            return true; // Complete request with body
        } else {
            return false; // Incomplete request, waiting for more body data
        }
    }

    // Check for Transfer-Encoding: chunked
    if (headers.find("Transfer-Encoding") != headers.end() &&
        headers["Transfer-Encoding"] == "chunked") {
        // Search for the end of the chunked transfer
        size_t chunkEnd = client.getRequest().find("0\r\n\r\n", bodyStart);
        if (chunkEnd != std::string::npos) {
            // If chunked body is complete, set it in the Request object
            client.getRequestObject()->setBody(client.getRequest().substr(bodyStart, chunkEnd - bodyStart));
            client.getRequestObject()->setBody(stripChunkedEncoding(client.getRequestObject()->getBody()));
            return true;
        } else {
            return false; // Incomplete request, waiting for more chunked data
        }
    }

    // If there's no Content-Length or chunked encoding, assume the request is complete
    return true;
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
    Socket clientSocket = client.getSocket();

    std::vector<Server> filteredServers;
    for (size_t i = 0; i < servers.size(); i++) {
        const Server& server = servers[i];
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
}

void Worker::handleClientData(Client &client) {
    char buffer[BUFFER_LENGHT];
    int bytesRead;
    std::string receivedData;

    try {
        while ((bytesRead = recv(client.getFd(), buffer, sizeof buffer, 0)) > 0) {
            receivedData = std::string(buffer, bytesRead);
            // std::cout << "received input : " << receivedData << std::endl;

            if (client.getRequestObject() == NULL) {
                client.setRequestObject(new Request(receivedData));
                completeHeaders(receivedData, client);
            } else if (client.getRequestObject()->getHeaders().size() == 0) {
                completeHeaders(client.getRequest(), client);
            }
            client.setRequest(client.getRequest().append(receivedData));
            if (isCompleteRequest(client) || bytesRead < BUFFER_LENGHT) {
                break;
            }
        }

        // std::cout << "bytes read : " << bytesRead << std::endl;
        // std::cout << "message got = " << client.getRequest() << std::endl;
        // std::cout << "Request = " << client.getRequestObject() << std::endl;

        if (bytesRead == 0) {
            epollHandler.removeFd(client.getFd());
            close(client.getFd());
            // std::cout << "Connection closed on socket " << client.getFd() << std::endl;
            std::cout << "Connection closed" << std::endl;
            clientSockets.erase(std::find(clientSockets.begin(), clientSockets.end(), client.getFd()));
            return ;
        } else if (bytesRead < 0) {
            return;
        } else {
            if (!client.hasServer()) {
                assignServerToClient(*client.getRequestObject(), client);
            }
            if (isCompleteRequest(client)) {
                // std::cout << "request is complete" << std::endl;
                client.setResponse(client.getRequestObject()->generateResponse(*client.getServer()));
                client.clearRequest();
                delete client.getRequestObject();
                client.setRequestObject(NULL);
            }
        }
    } catch (const InvalidHttpRequestException& e) {
        std::cerr << e.what() << std::endl;
        Response response(400, "Bad Request");
        response.setBodyFromFile(servers[0].getRoot() + servers[0].getErrorPage400());
        client.setResponse(response.generateResponse(client.getRequestObject()->getHeaders()["Cookie"]));
        client.clearRequest();
        if (client.getRequestObject() != NULL) {
            delete client.getRequestObject();
            client.setRequestObject(NULL);
        }
        epollHandler.removeFd(client.getFd());
        clientSockets.erase(std::find(clientSockets.begin(), clientSockets.end(), client.getFd()));
        close(client.getFd());
        // std::cout << "Connection closed on socket " << client.getFd() << std::endl;
        std::cout << "Connection closed" << std::endl;
    }
}

void Worker::handleWritableData(Client &client) {

    std::string& response = client.getResponse();

    if (!response.empty()) {
        int bytesSent = send(client.getFd(), response.c_str(), response.size(), 0);

        if (bytesSent <= 0) {
            epollHandler.removeFd(client.getFd());
            close(client.getFd());
            std::cout << "Connection closed" << std::endl;
            clientSockets.erase(std::find(clientSockets.begin(), clientSockets.end(), client.getFd()));
            return ;
        } else {
            response.erase(0, bytesSent);

            if (response.empty()) {
                client.clearResponse();
            }
        }
    }
}

std::vector<Socket> Worker::getListeningSockets() const {
    return listeningSockets;
}

std::vector<Client> Worker::getClientSockets() const {
    return clientSockets;
}

