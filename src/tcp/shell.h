#ifndef SHELL_H
#define SHELL_H

#include <stddef.h> // Add this header for size_t
#include "../config/config.h"

// Execute a shell command and return its output
int execute_command(const char* command, char* output, size_t output_size);

// Handle a client connection, processing commands
void handle_client(int client_socket, const Config* config);

#endif // SHELL_H
