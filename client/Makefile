CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc

# Platform detection
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
    TARGET = build/client.exe
    RM = cmd /C "if exist build rmdir /S /Q build"
    MKDIR = cmd /C "if not exist build mkdir build && if not exist build\obj mkdir build\obj"
    RUN = build\client.exe
else
    LDFLAGS =
    TARGET = build/client
    RM = rm -rf build
    MKDIR = mkdir -p build/obj
    RUN = ./build/client
endif

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SOURCES = src/main.c \
          src/network/connection.c \
          src/network/message_handler.c \
          src/network/lobby_handler.c \
          src/network/session_handler.c

# Object files
OBJECTS = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/connection.o \
          $(OBJ_DIR)/message_handler.o \
          $(OBJ_DIR)/lobby_handler.o \
          $(OBJ_DIR)/session_handler.o

# Header dependencies
HEADERS = src/network/connection.h \
          src/network/message_handler.h \
          src/network/lobby_handler.h \
          src/network/session_handler.h \
          src/utils/constants.h

# Main target
all: directories $(TARGET)

# Create directories
directories:
	$(MKDIR)

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful: $(TARGET)"

# Compile object files
$(OBJ_DIR)/main.o: src/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/connection.o: src/network/connection.c src/network/connection.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/message_handler.o: src/network/message_handler.c src/network/message_handler.h src/utils/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lobby_handler.o: src/network/lobby_handler.c src/network/lobby_handler.h $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/session_handler.o: src/network/session_handler.c src/network/session_handler.h $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	$(RM)
	@echo "Cleaned build directory"

# Run client
run: all
	$(RUN)

# Rebuild from scratch
rebuild: clean all

.PHONY: all clean run rebuild directories