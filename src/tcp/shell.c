#include "shell.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define PROMPT "shell> "

int execute_command(const char* command, char* output, size_t output_size) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        snprintf(output, output_size, "Error executing command: %s\n", command);
        return -1;
    }
    
    size_t total_bytes = 0;
    size_t bytes_read;
    char buffer[128];
    
    output[0] = '\0';
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe)) > 0) {
        buffer[bytes_read] = '\0';
        
        // Check if there's enough space in the output buffer
        if (total_bytes + bytes_read >= output_size - 1) {
            bytes_read = output_size - total_bytes - 1;
            if (bytes_read <= 0) break;
        }
        
        strcat(output, buffer);
        total_bytes += bytes_read;
    }
    
    int status = pclose(pipe);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    
    return -1;
}

void handle_client(int client_socket, const Config* config) {
    char buffer[config->buffer_size];
    char response[config->buffer_size];
    
    // Send welcome message
    snprintf(response, sizeof(response), "%s\n%s", config->motd, PROMPT);
    tcp_send(client_socket, response, strlen(response));
    
    while (1) {
        // Receive command from client
        int bytes = tcp_receive(client_socket, buffer, sizeof(buffer));
        if (bytes <= 0) {
            printf("Client disconnected\n");
            break;
        }
        
        // Remove trailing newline if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0) {
            const char* bye = "Goodbye!\n";
            tcp_send(client_socket, bye, strlen(bye));
            break;
        }
        
        if (config->debug_mode) {
            printf("Executing command: %s\n", buffer);
        }
        
        // Execute command and get response
        memset(response, 0, sizeof(response));
        execute_command(buffer, response, sizeof(response) - strlen(PROMPT) - 1);
        
        // Append prompt for the next command
        strcat(response, PROMPT);
        
        // Send response back to client
        tcp_send(client_socket, response, strlen(response));
    }
    
    tcp_close(client_socket);
}
