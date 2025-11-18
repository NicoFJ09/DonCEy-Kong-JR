CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc -O2
RAYLIB_FLAGS = -lraylib -lm

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32 $(RAYLIB_FLAGS) -lopengl32 -lgdi32 -lwinmm
    TARGET = build/client.exe
    RM = cmd /C "if exist build rmdir /S /Q build"
    MKDIR = cmd /C "if not exist build mkdir build && if not exist build\obj mkdir build\obj"
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LDFLAGS = $(RAYLIB_FLAGS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        LDFLAGS = $(RAYLIB_FLAGS) -lpthread -ldl -lrt -lX11
    endif
    TARGET = build/client
    RM = rm -rf build
    MKDIR = mkdir -p build/obj
endif

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Network layer
NETWORK_SOURCES = src/network/connection.c \
                  src/network/message_handler.c \
                  src/network/lobby_handler.c \
                  src/network/session_handler.c

# Rendering layer
RENDERING_SOURCES = src/rendering/sprite_manager.c \
                    src/rendering/animation.c \
                    src/rendering/renderer.c \
                    src/rendering/sprite_test.c

# Game screens layer
GAME_SOURCES = src/game/screens/ip_input_screen.c

MAIN_SOURCE = src/main.c
SOURCES = $(MAIN_SOURCE) $(NETWORK_SOURCES) $(RENDERING_SOURCES) $(GAME_SOURCES)

# Object files
NETWORK_OBJECTS = $(OBJ_DIR)/connection.o \
                  $(OBJ_DIR)/message_handler.o \
                  $(OBJ_DIR)/lobby_handler.o \
                  $(OBJ_DIR)/session_handler.o

RENDERING_OBJECTS = $(OBJ_DIR)/sprite_manager.o \
                    $(OBJ_DIR)/animation.o \
                    $(OBJ_DIR)/renderer.o \
                    $(OBJ_DIR)/sprite_test.o

GAME_OBJECTS = $(OBJ_DIR)/ip_input_screen.o

OBJECTS = $(OBJ_DIR)/main.o $(NETWORK_OBJECTS) $(RENDERING_OBJECTS) $(GAME_OBJECTS)

HEADERS = src/network/connection.h \
          src/network/message_handler.h \
          src/network/lobby_handler.h \
          src/network/session_handler.h \
          src/rendering/sprite_manager.h \
          src/rendering/animation.h \
          src/rendering/renderer.h \
          src/rendering/sprite_test.h \
          src/game/screens/ip_input_screen.h \
          src/utils/constants.h

all: directories $(TARGET)

directories:
	$(MKDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✓ Build successful: $(TARGET)"

$(OBJ_DIR)/main.o: src/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/connection.o: src/network/connection.c src/network/connection.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/message_handler.o: src/network/message_handler.c src/network/message_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lobby_handler.o: src/network/lobby_handler.c src/network/lobby_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/session_handler.o: src/network/session_handler.c src/network/session_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/sprite_manager.o: src/rendering/sprite_manager.c src/rendering/sprite_manager.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/animation.o: src/rendering/animation.c src/rendering/animation.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/renderer.o: src/rendering/renderer.c src/rendering/renderer.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/sprite_test.o: src/rendering/sprite_test.c src/rendering/sprite_test.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/ip_input_screen.o: src/game/screens/ip_input_screen.c src/game/screens/ip_input_screen.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM)
	@echo "✓ Cleaned"

run: all
	$(TARGET)

rebuild: clean all

.PHONY: all clean run rebuild directories