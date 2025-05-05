#include "main.h"
#include "config/config.h"
#include "tcp/tcp.h"
#include "tcp/shell.h"
#include "tcp/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// Structure to pass data to client handler thread
typedef struct {
    int client_socket;
    Config config;
} ClientData;

// Thread function to handle a client connection
void* client_thread(void* arg) {
    ClientData* data = (ClientData*)arg;
    handle_client(data->client_socket, &data->config);
    free(data);
    return NULL;
}

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -c, --config FILE    Path to the configuration file (default: %s)\n", DEFAULT_CONFIG_PATH);
    printf("  -h, --help           Display this help and exit\n");
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    const char* config_path = DEFAULT_CONFIG_PATH;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
            config_path = argv[++i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Set up signal handlers
    setup_signal_handlers();
    
    // Load configuration
    Config config;
    if (load_config(config_path, &config) != 0) {
        fprintf(stderr, "Failed to load configuration from %s\n", config_path);
        return 1;
    }
    
    print_config(&config);
    
    // Initialize TCP server
    int server_socket = tcp_server_init(&config);
    if (server_socket < 0) {
        fprintf(stderr, "Failed to initialize TCP server\n");
        return 1;
    }
    
    // Main server loop
    while (1) {
        log_message(LOG_INFO, "Waiting for incoming connections...");
        
        int client_socket = tcp_accept_client(server_socket);
        if (client_socket < 0) {
            if (!is_server_running()) {
                break;  // Server is shutting down
            }
            log_message(LOG_ERROR, "Failed to accept client connection");
            continue;
        }
        
        // Create a new thread to handle the client
        ClientData* data = malloc(sizeof(ClientData));
        data->client_socket = client_socket;
        data->config = config;
        
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_thread, data) != 0) {
            log_message(LOG_ERROR, "Failed to create client thread");
            free(data);
            tcp_close(client_socket);
        } else {
            pthread_detach(thread_id);
        }
    }
    
    // Clean up server
    tcp_server_cleanup(server_socket);
    log_message(LOG_INFO, "Server shutdown complete");
    
    return 0;
}
