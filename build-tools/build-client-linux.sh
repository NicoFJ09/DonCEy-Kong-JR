#!/bin/bash
set -e

echo "================================================"
echo " Building DonCEy Kong Jr Client for Linux"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLIENT_DIR="$PROJECT_ROOT/client"
DIST_DIR="$PROJECT_ROOT/dist/linux/DonCEy-Kong-Jr-Client"

# Clean previous builds
echo "→ Cleaning previous builds..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Build client
echo "→ Building client binary..."
cd "$CLIENT_DIR"
make clean
make

# Copy binary
echo "→ Copying binary..."
cp "$CLIENT_DIR/build/client" "$DIST_DIR/DonCEy-Kong-Jr"
chmod +x "$DIST_DIR/DonCEy-Kong-Jr"

# Copy assets
echo "→ Copying assets..."
cp -r "$CLIENT_DIR/assets" "$DIST_DIR/"

# Create launch script
echo "→ Creating launch script..."
cat > "$DIST_DIR/launch.sh" << 'EOF'
#!/bin/bash

# Get the directory where this script is located
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Change to that directory
cd "$DIR"

# Launch the game
./DonCEy-Kong-Jr
EOF

chmod +x "$DIST_DIR/launch.sh"

# Create README
echo "→ Creating README..."
cat > "$DIST_DIR/README.txt" << 'EOF'
============================================
 DonCEy Kong Jr - Cliente Linux
============================================

REQUISITOS:
-----------
- Sistema operativo Linux (64-bit)
- OpenGL 3.3+ compatible
- Bibliotecas del sistema: libGL, libX11, libXrandr, libXi

INSTALACIÓN:
-----------
No requiere instalación. Simplemente extrae todos los archivos
a una carpeta y ejecuta el launcher.

CÓMO EJECUTAR:
--------------
Método 1 (Recomendado):
  ./launch.sh

Método 2 (Directo):
  ./DonCEy-Kong-Jr

ESTRUCTURA:
-----------
DonCEy-Kong-Jr     → Ejecutable principal
launch.sh          → Script de lanzamiento
assets/            → Recursos del juego (sprites, fuentes, etc.)
README.txt         → Este archivo

DEPENDENCIAS:
-------------
Si el juego no inicia, instala las dependencias necesarias:

Ubuntu/Debian:
  sudo apt-get install libgl1 libx11-6 libxrandr2 libxi6

Fedora/RHEL:
  sudo dnf install mesa-libGL libX11 libXrandr libXi

Arch:
  sudo pacman -S libgl libx11 libxrandr libxi

SOLUCIÓN DE PROBLEMAS:
----------------------
• Error "cannot execute binary file":
  → Asegúrate de que estás en un sistema Linux de 64 bits
  → Verifica permisos: chmod +x DonCEy-Kong-Jr

• Error "error while loading shared libraries":
  → Instala las dependencias del sistema (ver sección DEPENDENCIAS)

• Pantalla negra o crash:
  → Verifica que tu GPU soporte OpenGL 3.3+
  → Actualiza drivers de video

• No encuentra assets:
  → Asegúrate de que la carpeta assets/ está en el mismo directorio

SOPORTE:
--------
Para reportar bugs o solicitar ayuda, contacta al equipo de desarrollo.

================================================
© 2024 DonCEy Kong Jr Team
================================================
EOF

# Create .desktop file (optional)
echo "→ Creating .desktop file..."
cat > "$DIST_DIR/doncey-kong-jr.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=DonCEy Kong Jr
Comment=Classic arcade game remake
Exec=./launch.sh
Icon=./assets/icons/linux/client.png
Terminal=false
Categories=Game;ArcadeGame;
EOF

echo ""
echo "✓ Linux Client build complete!"
echo "  Location: $DIST_DIR"
echo ""
echo "To test:"
echo "  cd '$DIST_DIR' && ./launch.sh"
echo ""
