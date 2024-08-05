#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3490
#define SERVER_IP "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    const char *message = "Hello from client!";
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // // Connect to the server
    // if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    //     perror("Connection Failed");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }

    // Send the message to the server
    if (send(sockfd, message, strlen(message), 0))
    	printf("Message sent to server: %s\n", message);
	else
		printf("error sending data to server\n");

    // Read the server's response
    read(sockfd, buffer, sizeof(buffer));
    printf("Server response: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}
