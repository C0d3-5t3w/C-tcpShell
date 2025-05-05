# TCP Shell Makefile

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -O2
LDFLAGS = -pthread

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/config/config.c \
          $(SRC_DIR)/tcp/tcp.c \
          $(SRC_DIR)/tcp/shell.c \
          $(SRC_DIR)/tcp/utils.c

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Binary name
TARGET = $(BIN_DIR)/tcp_shell

# Targets
.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Build complete! Run with: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)/config $(OBJ_DIR)/tcp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/config $(OBJ_DIR)/tcp:
	mkdir -p $@

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete!"

# Install target
install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin
	install -d $(DESTDIR)/etc/tcp_shell
	install -m 644 config.yaml $(DESTDIR)/etc/tcp_shell
	@echo "Installation complete!"

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all      - Build the project"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Run the TCP shell"
	@echo "  install   - Install the binary and config file"
	@echo "  help     - Show this help message"