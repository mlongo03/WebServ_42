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

#define PORT "3490"  // the port users will be connecting to
#define IP "127.0.0.1"  // the IP users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

int sockfd;

void	signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		close(sockfd);
		exit (1);
	}
	// else if (signum == SIGTERM)
	// 	exit (1);
}

int main(void) {
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    int new_fd, status;
    char ip_str[INET6_ADDRSTRLEN];
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

	signal(SIGINT, signal_handler);

    printf("Server listening on %s:%s\n", IP, PORT);

	while(1) {  // main accept() loop
		addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

        // Print the IP address and port of the connected client
        if (their_addr.ss_family == AF_INET) {
            // IPv4
            struct sockaddr_in *s = (struct sockaddr_in *)&their_addr;
            inet_ntop(AF_INET, &s->sin_addr, ip_str, sizeof ip_str);
            printf("Accepted connection from %s:%d\n", ip_str, ntohs(s->sin_port));
        } else { // AF_INET6
            // IPv6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&their_addr;
            inet_ntop(AF_INET6, &s->sin6_addr, ip_str, sizeof ip_str);
            printf("Accepted connection from %s:%d\n", ip_str, ntohs(s->sin6_port));
        }

		if (!fork()) { // this is the child process
			// Read and respond to the client
			char buffer[1024] = {0};
			read(new_fd, buffer, sizeof buffer);
			printf("message received: %s\n", buffer);
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

    // Close the connection
    close(sockfd);

    return 0;
}

