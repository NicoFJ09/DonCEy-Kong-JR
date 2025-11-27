#!/bin/bash
set -e

echo "================================================"
echo " DonCEy Kong Jr - Build All Platforms"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_TOOLS="$PROJECT_ROOT/build-tools"

# Detect OS
OS="unknown"
case "$OSTYPE" in
    darwin*)  OS="macos" ;;
    linux*)   OS="linux" ;;
    msys*|cygwin*|win32) OS="windows" ;;
esac

echo "Detected OS: $OS"
echo ""

# Make all scripts executable
echo "→ Making build scripts executable..."
chmod +x "$BUILD_TOOLS"/*.sh 2>/dev/null || true

# Track success/failure
BUILD_SUCCESS=()
BUILD_FAILED=()

# Build native client
echo "================================================"
echo " Building Client for $OS"
echo "================================================"
if [ -f "$BUILD_TOOLS/build-client-$OS.sh" ]; then
    if bash "$BUILD_TOOLS/build-client-$OS.sh"; then
        BUILD_SUCCESS+=("Client ($OS)")
    else
        BUILD_FAILED+=("Client ($OS)")
    fi
else
    echo "⚠ No build script for $OS client"
    BUILD_FAILED+=("Client ($OS) - no script")
fi

echo ""
echo "================================================"
echo " Building Server for $OS"
echo "================================================"

# Build server
case "$OS" in
    macos|linux)
        if [ -f "$BUILD_TOOLS/build-server-$OS.sh" ]; then
            if bash "$BUILD_TOOLS/build-server-$OS.sh"; then
                BUILD_SUCCESS+=("Server ($OS)")
            else
                BUILD_FAILED+=("Server ($OS)")
            fi
        else
            echo "⚠ No build script for $OS server"
            BUILD_FAILED+=("Server ($OS) - no script")
        fi
        ;;
    windows)
        if [ -f "$BUILD_TOOLS/build-server-windows.bat" ]; then
            if cmd //c "$BUILD_TOOLS\\build-server-windows.bat"; then
                BUILD_SUCCESS+=("Server (Windows)")
            else
                BUILD_FAILED+=("Server (Windows)")
            fi
        else
            echo "⚠ No build script for Windows server"
            BUILD_FAILED+=("Server (Windows) - no script")
        fi
        ;;
esac

echo ""
echo "================================================"
echo " Build Summary"
echo "================================================"

if [ ${#BUILD_SUCCESS[@]} -gt 0 ]; then
    echo "✓ Successful builds:"
    for build in "${BUILD_SUCCESS[@]}"; do
        echo "  - $build"
    done
fi

if [ ${#BUILD_FAILED[@]} -gt 0 ]; then
    echo ""
    echo "✗ Failed builds:"
    for build in "${BUILD_FAILED[@]}"; do
        echo "  - $build"
    done
fi

echo ""
echo "Distribution directory: $PROJECT_ROOT/dist/$OS/"
echo ""

# Optional: Try cross-compilation for Windows if on macOS/Linux
if [ "$OS" != "windows" ]; then
    echo "================================================"
    echo " Optional: Cross-compile for Windows?"
    echo "================================================"
    echo ""
    echo "To build Windows executables from $OS, you can run:"
    echo "  bash build-tools/build-client-windows.sh"
    echo ""
    echo "Requirements:"
    echo "  - mingw-w64 toolchain"
    echo "  - Windows Raylib libraries"
    echo ""
fi

exit 0
