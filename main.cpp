#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT "3490"  // the port users will be connecting to
#define IP "127.0.0.1"  // the IP users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

int main(void) {
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd, status;
    char ip_str[INET6_ADDRSTRLEN];

    // Load up address structs with getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(IP, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Make a socket
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("socket failed");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    // Bind it
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind failed");
        close(sockfd);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res); // free the linked-list

    // Listen on it
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%s\n", IP, PORT);

    // Accept an incoming connection
    addr_size = sizeof their_addr;
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
        perror("accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
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

    // Read and respond to the client
    char buffer[1024] = {0};
    read(new_fd, buffer, sizeof buffer);
    printf("Received: %s\n", buffer);
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
    write(new_fd, response, strlen(response));

    // Close the connection
    close(new_fd);
    close(sockfd);

    return 0;
}

