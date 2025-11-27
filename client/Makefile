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
    LDFLAGS = -lraylib -lm -lGL -lX11 -lXrandr -lXi -lpthread -ldl
    TARGET = $(BUILD_DIR)/client
    MKDIR = mkdir -p
    RM = rm -rf
endif

ifeq ($(UNAME_S),Darwin)
    # macOS: Buscar Raylib en múltiples ubicaciones
    RAYLIB_PATHS = /usr/local/lib /usr/local/Cellar/raylib/5.5/lib /opt/homebrew/lib
    RAYLIB_INCLUDE_PATHS = /usr/local/include /usr/local/Cellar/raylib/5.5/include /opt/homebrew/include
    
    # Encontrar la primera ruta válida
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
    
    TARGET = $(BUILD_DIR)/client
    MKDIR = mkdir -p
    RM = rm -rf
endif

ifeq ($(UNAME_S),Windows_NT)
    # Windows: TODAS las librerías en orden correcto
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 -lcrypt32 -lm
    TARGET = $(BUILD_DIR)/client.exe
    MKDIR = powershell -Command "New-Item -ItemType Directory -Force -Path"
    RM = powershell -Command "Remove-Item -Recurse -Force -ErrorAction SilentlyContinue"
endif

# Source files (organized by module)
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

.PHONY: all clean run test-raylib help

all: $(TARGET)

# Test Raylib installation
test-raylib:
	@echo "Testing Raylib installation..."
	@echo "Platform: $(UNAME_S)"
ifeq ($(UNAME_S),Darwin)
	@echo "Checking Raylib paths..."
	@echo "Library paths: $(RAYLIB_PATHS)"
	@echo "Include paths: $(RAYLIB_INCLUDE_PATHS)"
	@echo "Found library: $(RAYLIB_LIB_DIR)"
	@echo "Found headers: $(RAYLIB_INCLUDE_DIR)"
endif
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "TARGET: $(TARGET)"

# Link executable (CRITICAL: $@ expands to full target path)
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✓ Build complete: $(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: src/%.c
	@echo "Compiling $<..."
ifeq ($(UNAME_S),Windows_NT)
	@powershell -Command "New-Item -ItemType Directory -Force -Path $(OBJ_DIR) | Out-Null"
	@powershell -Command "New-Item -ItemType Directory -Force -Path $(dir $@) | Out-Null"
else
	@mkdir -p $(dir $@)
endif
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
ifeq ($(UNAME_S),Windows_NT)
	@powershell -Command "if (Test-Path $(BUILD_DIR)) { Remove-Item -Recurse -Force $(BUILD_DIR) }"
else
	@rm -rf $(BUILD_DIR)
endif
	@echo "✓ Cleaned"

# Run the client
run: all
	@echo "Running client..."
ifeq ($(UNAME_S),Windows_NT)
	@cd $(BUILD_DIR) && client.exe
else
	@cd $(BUILD_DIR) && ./client
endif

# Help
help:
	@echo "DonCEy Kong Jr - Client Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all (default) - Build client executable"
	@echo "  clean         - Remove build artifacts"
	@echo "  run           - Build and run client"
	@echo "  test-raylib   - Test Raylib installation"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Platform: $(UNAME_S)"
	@echo "TARGET: $(TARGET)"