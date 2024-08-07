#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define PORT "3490"  // the port users will be connecting to
#define IP "127.0.0.1"  // the IP users will be connecting to
#define PORT2 "3030"
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_EVENTS 10  // Maximum number of events to be returned by epoll_wait

int sockfd;
int sockfd2;
int epoll_fd;

// Return a listening socket
int get_listener_socket(const char *ip, const char *port)
{
    int listener;
    struct addrinfo hints, *res, *p;
    int status;
    int yes=1;

    // Load up address structs with getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    // loop through all the results and bind to the first we can
    for(p = res; p != NULL; p = p->ai_next) {

        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        // an option that permits a socket to use an already used socket (fd)
        // it permits to use a socket used earlier fast and to not wait for it to close
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
            close(listener);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(res); // free the linked-list
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    // Listen on it
    if (listen(listener, BACKLOG) == -1) {
        perror("listen failed");
        close(listener);
        exit(EXIT_FAILURE);
    }
    return listener;
}

// Set a file descriptor to non-blocking mode
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        close(sockfd);
        close(sockfd2);
        close(epoll_fd);
        exit(1);
    }
}

int main()
{
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    int newfd;
    char ip_str[INET6_ADDRSTRLEN];
    char buf[256]; // Buffer for client data
    int nbytes;
    struct epoll_event ev, events[MAX_EVENTS];

    sockfd = get_listener_socket(IP, PORT);
    sockfd2 = get_listener_socket(IP, PORT2);

    if (sockfd == -1 || sockfd2 == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Create an epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Add the sockfd to the epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("epoll_ctl: sockfd");
        exit(EXIT_FAILURE);
    }

    // Add the sockfd2 to the epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = sockfd2;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd2, &ev) == -1) {
        perror("epoll_ctl: sockfd2");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signal_handler);

    printf("Server listening on %s:%s\n", IP, PORT);
    printf("Server listening on %s:%s\n", IP, PORT2);

    while(1) { // main loop
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd || events[i].data.fd == sockfd2) {
                // Handle new connections
                addrlen = sizeof remoteaddr;
                newfd = accept(events[i].data.fd, (struct sockaddr *)&remoteaddr, &addrlen);

                if (newfd == -1) {
                    perror("accept");
                } else {
                    set_nonblocking(newfd);

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = newfd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newfd, &ev) == -1) {
                        perror("epoll_ctl: newfd");
                        exit(EXIT_FAILURE);
                    }

                    if (remoteaddr.ss_family == AF_INET) {
                        struct sockaddr_in *s = (struct sockaddr_in *)&remoteaddr;
                        inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof ip_str);
                        printf("Accepted connection from %s:%d on "
                            "socket %d, associated to fd %d\n", ip_str, ntohs(s->sin_port), events[i].data.fd, newfd);
                    } else {
                        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&remoteaddr;
                        inet_ntop(AF_INET6, &s->sin6_addr, ip_str, sizeof ip_str);
                        printf("Accepted connection from %s:%d on "
                            "socket %d, associated to fd %d\n", ip_str, ntohs(s->sin6_port), events[i].data.fd, newfd);
                    }
                }
            } else {
                // Handle data from clients
                nbytes = recv(events[i].data.fd, buf, sizeof buf, 0);
                if (nbytes <= 0) {
                    if (nbytes == 0) {
                        // Connection closed
                        printf("pollserver: fd %d hung up\n", events[i].data.fd);
                    } else {
                        perror("recv");
                    }
                    close(events[i].data.fd);
                } else {
                    // We got some good data from a client
                    printf("message received from fd %d: %s\n", events[i].data.fd, buf);
                    for (int j = 0; j < nfds; j++) {
                        int dest_fd = events[j].data.fd;
                        if (dest_fd != sockfd && dest_fd != sockfd2 && dest_fd != events[i].data.fd) {
                            if (send(dest_fd, buf, nbytes, 0) == -1) {
                                perror("send");
                            }
                        }
                    }
                }
            }
        }
    }

    // Close the connections
    close(sockfd);
    close(sockfd2);
    close(epoll_fd);

    return 0;
}


