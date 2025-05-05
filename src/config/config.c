#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Parse a line in the format "key: value"
static int parse_line(char* line, char** key, char** value) {
    char* sep = strchr(line, ':');
    if (!sep) return 0;
    
    *sep = '\0';
    *key = line;
    *value = sep + 1;
    
    // Trim whitespace from key
    while (isspace(**key)) (*key)++;
    char* end = *key + strlen(*key) - 1;
    while (end > *key && isspace(*end)) *end-- = '\0';
    
    // Trim whitespace from value
    while (isspace(**value)) (*value)++;
    end = *value + strlen(*value) - 1;
    while (end > *value && isspace(*end)) *end-- = '\0';
    
    return 1;
}

int load_config(const char* config_path, Config* config) {
    FILE* file = fopen(config_path, "r");
    if (!file) {
        perror("Error opening config file");
        return -1;
    }
    
    // Set default values
    strcpy(config->host, "0.0.0.0");
    config->port = 4444;
    config->backlog = 5;
    config->buffer_size = 4096;
    config->timeout_seconds = 60;
    strcpy(config->motd, "Welcome to TCP Shell");
    config->debug_mode = 0;
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char* key;
        char* value;
        
        if (parse_line(line, &key, &value)) {
            if (strcmp(key, "host") == 0) {
                strncpy(config->host, value, sizeof(config->host) - 1);
            } else if (strcmp(key, "port") == 0) {
                config->port = atoi(value);
            } else if (strcmp(key, "backlog") == 0) {
                config->backlog = atoi(value);
            } else if (strcmp(key, "buffer_size") == 0) {
                config->buffer_size = atoi(value);
            } else if (strcmp(key, "timeout_seconds") == 0) {
                config->timeout_seconds = atoi(value);
            } else if (strcmp(key, "motd") == 0) {
                strncpy(config->motd, value, sizeof(config->motd) - 1);
            } else if (strcmp(key, "debug_mode") == 0) {
                config->debug_mode = (strcmp(value, "true") == 0 || atoi(value) == 1);
            }
        }
    }
    
    fclose(file);
    return 0;
}

void print_config(const Config* config) {
    printf("Configuration:\n");
    printf("  Host: %s\n", config->host);
    printf("  Port: %d\n", config->port);
    printf("  Backlog: %d\n", config->backlog);
    printf("  Buffer size: %d\n", config->buffer_size);
    printf("  Timeout: %d seconds\n", config->timeout_seconds);
    printf("  MOTD: %s\n", config->motd);
    printf("  Debug mode: %s\n", config->debug_mode ? "on" : "off");
}
