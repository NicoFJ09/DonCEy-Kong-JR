CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc

# Platform detection
ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    LDFLAGS = -lws2_32
    TARGET = build/client.exe
    RM = rmdir /S /Q
    MKDIR = if not exist build mkdir build && if not exist build\obj mkdir build\obj
    RUN = build\client.exe
else
    # Unix/Linux/macOS settings
    LDFLAGS =
    TARGET = build/client
    RM = rm -rf
    MKDIR = mkdir -p build build/obj
    RUN = ./build/client
endif

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SOURCES = src/main.c \
          src/network/connection.c

# Object files
OBJECTS = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/connection.o

# Main target
all: directories $(TARGET)

# Create directories
directories:
	$(MKDIR)

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✓ Build successful: $(TARGET)"

# Compile main.o
$(OBJ_DIR)/main.o: src/main.c src/network/connection.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile connection.o
$(OBJ_DIR)/connection.o: src/network/connection.c src/network/connection.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	$(RM) build
	@echo "✓ Cleaned build directory"

# Run client
run: all
	$(RUN)

# Rebuild from scratch
rebuild: clean all

.PHONY: all clean run rebuild directories