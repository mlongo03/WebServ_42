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
#include <poll.h>

#define PORT "3490"  // the port users will be connecting to
#define IP "127.0.0.1"  // the IP users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
int sockfd;

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    struct addrinfo hints, *res, *p;
    int status;
	int yes=1;

    // Load up address structs with getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(IP, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

	// loop through all the results and bind to the first we can
	for(p = res; p != NULL; p = p->ai_next) {

		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		//an option that permit to a socket to use an already
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
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
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = (pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void	signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		close(sockfd);
		exit (1);
	}
}

int main(void)
{
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    int newfd;
    char ip_str[INET6_ADDRSTRLEN];
    int fd_count = 0; // Start off with room for 5 connections
    int fd_size = 5; // (We'll realloc as necessary)
    struct pollfd *pfds = (pollfd *)malloc(sizeof *pfds * fd_size);
    char buf[256];    // Buffer for client data
    char remoteIP[INET6_ADDRSTRLEN];

	sockfd = get_listener_socket();
    if (sockfd == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the sockfd to set
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the sockfd

	signal(SIGINT, signal_handler);
    printf("Server listening on %s:%s\n", IP, PORT);

	while(1) {  // main loop
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fd_count; i++) {

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN) { // We got one!!

                if (pfds[i].fd == sockfd) {
                    // If sockfd is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(sockfd,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

					if (remoteaddr.ss_family == AF_INET) {
						// IPv4
						struct sockaddr_in *s = (struct sockaddr_in *)&remoteaddr;
						inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof ip_str);
						printf("Accepted connection from %s:%d\n", ip_str, ntohs(s->sin_port));
					} else { // AF_INET6
						// IPv6
						struct sockaddr_in6 *s = (struct sockaddr_in6 *)&remoteaddr;
						inet_ntop(AF_INET6, &s->sin6_addr, ip_str, sizeof ip_str);
						printf("Accepted connection from %s:%d\n", ip_str, ntohs(s->sin6_port));
					}

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                            "socket %d, associated to fd %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN), pfds[i].fd,
                            newfd);
                    }
                } else {
                    // If not the sockfd, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            printf("pollserver: fd %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        del_from_pfds(pfds, i, &fd_count);

                    } else {
                        // We got some good data from a client
						printf("message received from fd %d: %s\n", pfds[i].fd, buf);
                        for(int j = 0; j < fd_count; j++) {
                            // Send to everyone!
                            int dest_fd = pfds[j].fd;

                            // Except the sockfd and ourselves
                            if (dest_fd != sockfd && dest_fd != sender_fd) {
                                if (send(dest_fd, buf, nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    // Close the connection
    close(sockfd);

    return 0;
}

