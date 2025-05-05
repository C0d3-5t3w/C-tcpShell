#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// Log levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

// Log a message with the specified level
void log_message(LogLevel level, const char* format, ...);

// Setup signal handlers
void setup_signal_handlers();

// Check if the server is still running
int is_server_running();

#endif // UTILS_H
