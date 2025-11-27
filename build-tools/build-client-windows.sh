#!/bin/bash
set -e

echo "================================================"
echo " Cross-Compiling DonCEy Kong Jr Client for Windows"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLIENT_DIR="$PROJECT_ROOT/client"
DIST_DIR="$PROJECT_ROOT/dist/windows/DonCEy-Kong-Jr-Client"

# Check for mingw-w64
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "ERROR: mingw-w64 no está instalado."
    echo ""
    echo "Instalación según tu sistema operativo:"
    echo ""
    echo "macOS:"
    echo "  brew install mingw-w64"
    echo ""
    echo "Ubuntu/Debian:"
    echo "  sudo apt-get install mingw-w64"
    echo ""
    echo "Fedora:"
    echo "  sudo dnf install mingw64-gcc"
    echo ""
    echo "Arch:"
    echo "  sudo pacman -S mingw-w64-gcc"
    echo ""
    exit 1
fi

# Clean previous builds
echo "→ Cleaning previous builds..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Build for Windows
echo "→ Cross-compiling for Windows..."
cd "$CLIENT_DIR"

# Compile object files
echo "  • Compiling source files..."
x86_64-w64-mingw32-gcc -c src/main.c -o build/obj/main.o \
    -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
    -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall

x86_64-w64-mingw32-gcc -c src/external/cJSON.c -o build/obj/cJSON.o \
    -std=c99 -Wall

# Compile all game module files
for file in src/game/*.c; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .c)
    echo "    - $filename.c"
    x86_64-w64-mingw32-gcc -c "$file" -o "build/obj/$filename.o" \
        -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
        -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
done

# Compile all game/screens module files
for file in src/game/screens/*.c; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .c)
    echo "    - screens/$filename.c"
    x86_64-w64-mingw32-gcc -c "$file" -o "build/obj/$filename.o" \
        -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
        -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
done

# Compile all network module files
for file in src/network/*.c; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .c)
    echo "    - network/$filename.c"
    x86_64-w64-mingw32-gcc -c "$file" -o "build/obj/$filename.o" \
        -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
        -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
done

# Compile all rendering module files
for file in src/rendering/*.c; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .c)
    echo "    - rendering/$filename.c"
    x86_64-w64-mingw32-gcc -c "$file" -o "build/obj/$filename.o" \
        -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
        -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
done

# Compile all utils module files
for file in src/utils/*.c; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .c)
    echo "    - utils/$filename.c"
    x86_64-w64-mingw32-gcc -c "$file" -o "build/obj/$filename.o" \
        -I/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/include \
        -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
done

# Link (you'll need Windows versions of Raylib libraries)
echo "  • Linking executable..."
x86_64-w64-mingw32-gcc build/obj/*.o -o "$DIST_DIR/DonCEy-Kong-Jr.exe" \
    -L/opt/homebrew/Cellar/mingw-w64/*/toolchain-x86_64/x86_64-w64-mingw32/lib \
    -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 \
    -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic \
    -mwindows

# Copy assets
echo "→ Copying assets..."
cp -r "$CLIENT_DIR/assets" "$DIST_DIR/"

# Create README
echo "→ Creating README..."
cat > "$DIST_DIR/README.txt" << 'EOF'
============================================
 DonCEy Kong Jr - Cliente Windows
============================================

REQUISITOS:
-----------
- Windows 7 o superior (64-bit)
- OpenGL 3.3+ compatible (drivers de video actualizados)

INSTALACIÓN:
-----------
No requiere instalación. Simplemente extrae todos los archivos
a una carpeta y ejecuta el launcher.

CÓMO EJECUTAR:
--------------
Doble-click en: DonCEy-Kong-Jr.exe

ESTRUCTURA:
-----------
DonCEy-Kong-Jr.exe → Ejecutable principal
assets/            → Recursos del juego (sprites, fuentes, etc.)
README.txt         → Este archivo

SOLUCIÓN DE PROBLEMAS:
----------------------
• Error "OpenGL 3.3 not supported":
  → Actualiza los drivers de tu tarjeta gráfica
  → Visita el sitio web del fabricante (NVIDIA, AMD, Intel)

• Error "VCRUNTIME140.dll not found":
  → Instala Visual C++ Redistributable:
  → https://aka.ms/vs/17/release/vc_redist.x64.exe

• Pantalla negra o crash al iniciar:
  → Verifica que tu GPU soporte OpenGL 3.3+
  → Actualiza DirectX: https://www.microsoft.com/download/directx

• No encuentra assets:
  → Asegúrate de que la carpeta assets/ está en el mismo directorio
  → No muevas el .exe fuera de su carpeta

• Firewall bloquea conexiones:
  → Windows puede pedir permiso la primera vez
  → Click en "Permitir acceso" para conexiones de red

CONTROLES:
----------
Flechas: Movimiento
Espacio: Saltar
ESC: Menú

CONECTAR AL SERVIDOR:
---------------------
1. Inicia el juego
2. Ingresa la IP del servidor
3. Si es local: 127.0.0.1
4. Si es red local: IP de la PC servidor (ej: 192.168.1.X)
5. Puerto por defecto: 5000

SOPORTE:
--------
Para reportar bugs o solicitar ayuda, contacta al equipo de desarrollo.

================================================
© 2024 DonCEy Kong Jr Team
================================================
EOF

# Note about icon (requires rcedit on Windows or Wine)
echo ""
echo "✓ Windows Client build complete!"
echo "  Location: $DIST_DIR"
echo ""
echo "NOTA: Para agregar icono al .exe:"
echo "  1. Instala rcedit: npm install -g rcedit"
echo "  2. Ejecuta: rcedit DonCEy-Kong-Jr.exe --set-icon assets/icons/windows/client.ico"
echo ""
echo "NOTA: Este build fue cross-compilado. Pruébalo en una PC Windows real."
echo ""
