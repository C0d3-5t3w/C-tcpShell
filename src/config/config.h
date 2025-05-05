#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char host[256];
    int port;
    int backlog;
    int buffer_size;
    int timeout_seconds;
    char motd[1024];  // Message of the day
    int debug_mode;
} Config;

// Load configuration from the config.yaml file
int load_config(const char* config_path, Config* config);

// Print the current configuration
void print_config(const Config* config);

#endif // CONFIG_H
