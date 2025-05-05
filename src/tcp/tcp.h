#ifndef TCP_H
#define TCP_H

#include <stddef.h> // Add this header for size_t
#include "../config/config.h"

// Initialize the TCP server using the configuration
int tcp_server_init(const Config* config);

// Accept a new client connection
int tcp_accept_client(int server_socket);

// Send data to a client
int tcp_send(int client_socket, const char* data, size_t length);

// Receive data from a client
int tcp_receive(int client_socket, char* buffer, size_t buffer_size);

// Close a socket connection
void tcp_close(int socket);

// Clean up the server
void tcp_server_cleanup(int server_socket);

#endif // TCP_H
