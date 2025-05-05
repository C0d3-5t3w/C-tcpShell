#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>

static int server_running = 1;

// Signal handler for graceful shutdown
static void handle_signal(int sig) {
    printf("\nReceived signal %d, shutting down...\n", sig);
    server_running = 0;
}

void setup_signal_handlers() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE (client disconnects)
}

int is_server_running() {
    return server_running;
}

void log_message(LogLevel level, const char* format, ...) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[20];
    
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    const char* level_str;
    FILE* output = stdout;
    
    switch(level) {
        case LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case LOG_INFO:
            level_str = "INFO";
            break;
        case LOG_WARN:
            level_str = "WARN";
            break;
        case LOG_ERROR:
            level_str = "ERROR";
            output = stderr;
            break;
        default:
            level_str = "UNKNOWN";
    }
    
    fprintf(output, "[%s] [%s] ", timestamp, level_str);
    
    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
    
    fprintf(output, "\n");
    fflush(output);
}
