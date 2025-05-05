#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

int tcp_server_init(const Config* config) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }
    
    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    
    // Set up the socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(config->host);
    address.sin_port = htons(config->port);
    
    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    
    // Listen for connections
    if (listen(server_fd, config->backlog) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    
    printf("Server listening on %s:%d\n", config->host, config->port);
    return server_fd;
}

int tcp_accept_client(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    }
    
    printf("New connection from %s:%d\n", 
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    return client_socket;
}

int tcp_send(int client_socket, const char* data, size_t length) {
    ssize_t bytes_sent = send(client_socket, data, length, 0);
    if (bytes_sent < 0) {
        perror("send");
        return -1;
    }
    return bytes_sent;
}

int tcp_receive(int client_socket, char* buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(client_socket, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return -1;
    } else if (bytes_received == 0) {
        // Connection closed
        return 0;
    }
    
    // Null-terminate the received data
    buffer[bytes_received] = '\0';
    return bytes_received;
}

void tcp_close(int socket) {
    close(socket);
}

void tcp_server_cleanup(int server_socket) {
    close(server_socket);
    printf("Server socket closed\n");
}
