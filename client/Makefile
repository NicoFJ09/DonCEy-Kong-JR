# ============================================================
# DonCEy Kong Jr - Client Makefile
# Cross-platform build system (Linux, macOS, Windows)
# ============================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc -O2

# Platform detection
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows_NT)

# Directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    # Static linking for Raylib on Linux
    LDFLAGS = -lraylib -lm -lpthread -ldl
    LDFLAGS += -lGL -lX11 -lXrandr -lXi -lXcursor -lXinerama
    TARGET_NAME = client
endif

ifeq ($(UNAME_S),Darwin)
    RAYLIB_PATHS = /usr/local/lib /usr/local/Cellar/raylib/5.5/lib /opt/homebrew/lib
    RAYLIB_INCLUDE_PATHS = /usr/local/include /usr/local/Cellar/raylib/5.5/include /opt/homebrew/include
    
    RAYLIB_LIB_DIR := $(firstword $(foreach dir,$(RAYLIB_PATHS),$(wildcard $(dir)/libraylib.a)))
    RAYLIB_INCLUDE_DIR := $(firstword $(foreach dir,$(RAYLIB_INCLUDE_PATHS),$(wildcard $(dir)/raylib.h)))
    
    ifneq ($(RAYLIB_LIB_DIR),)
        LDFLAGS := -L$(dir $(RAYLIB_LIB_DIR)) -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        LDFLAGS := -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    endif
    
    ifneq ($(RAYLIB_INCLUDE_DIR),)
        CFLAGS += -I$(dir $(RAYLIB_INCLUDE_DIR))
    endif
    
    TARGET_NAME = client
endif

ifeq ($(UNAME_S),Windows_NT)
    # Windows linking - order matters!
    # Link against static raylib, but don't force all libs to be static
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 -lm
    LDFLAGS += -static-libgcc
    TARGET_NAME = client.exe
endif

# Full path to target
TARGET = $(BUILD_DIR)/$(TARGET_NAME)

# Source files
SRCS = src/main.c \
       src/external/cJSON.c \
       src/game/game_flow.c \
       src/game/level.c \
       src/game/player.c \
       src/game/enemy.c \
       src/game/fruit.c \
       src/game/screens/ip_input_screen.c \
       src/game/screens/title_screen.c \
       src/game/screens/player_screen.c \
       src/game/screens/lose_screen.c \
       src/game/screens/player_selection_screen.c \
       src/game/screens/spectator_screen.c \
       src/network/connection.c \
       src/network/game_events.c \
       src/rendering/renderer.c \
       src/rendering/sprite_manager.c \
       src/rendering/sprite_test.c \
       src/rendering/animation.c \
       src/utils/font_manager.c \
       src/utils/message_listener.c

# Object files
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# ============================================================
# TARGETS
# ============================================================

.PHONY: all clean run help

all: $(TARGET)

# Link executable (CRITICAL: LDFLAGS va AL FINAL)
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build complete: $(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: src/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned"

# Run
run: all
	@echo "Running $(TARGET_NAME)..."
	@cd $(BUILD_DIR) && ./$(TARGET_NAME)

# Help
help:
	@echo "DonCEy Kong Jr - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build executable"
	@echo "  clean  - Remove build artifacts"
	@echo "  run    - Build and run"
	@echo "  help   - Show this message"
	@echo ""
	@echo "Platform: $(UNAME_S)"
	@echo "Target: $(TARGET)"