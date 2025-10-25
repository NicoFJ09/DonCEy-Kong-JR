CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SOURCES = src/main.c \
          src/network/connection.c

# Object files
OBJECTS = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/connection.o

# Executable
TARGET = $(BUILD_DIR)/client

# Main target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@
	@echo "✓ Build successful: $(TARGET)"

# Compile main.o
$(OBJ_DIR)/main.o: src/main.c src/network/connection.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile connection.o
$(OBJ_DIR)/connection.o: src/network/connection.c src/network/connection.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build directory"

# Run client
run: all
	./$(TARGET)

# Rebuild from scratch
rebuild: clean all

.PHONY: all clean run rebuild directories