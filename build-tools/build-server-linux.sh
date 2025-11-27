#!/bin/bash
set -e

echo "================================================"
echo " Building DonCEy Kong Jr Server for Linux"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="$PROJECT_ROOT/server"
DIST_DIR="$PROJECT_ROOT/dist/linux/DonCEy-Kong-Jr-Server"

# Clean previous builds
echo "→ Cleaning previous builds..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Build server
echo "→ Building server..."
cd "$PROJECT_ROOT"
rm -rf server/bin
mkdir -p server/bin

javac -d server/bin \
    server/src/Main.java \
    server/src/network/*.java \
    server/src/gamestate/*.java \
    server/src/ui/*.java \
    server/src/utils/*.java \
    server/src/game/*.java

# Create JAR
echo "→ Creating JAR..."
cd server/bin
cat > MANIFEST.MF << EOF
Manifest-Version: 1.0
Main-Class: server.src.Main
EOF

jar cfm DonCEy-Kong-Jr-Server.jar MANIFEST.MF $(find . -name '*.class')
cd ../..

# Copy JAR to distribution
echo "→ Copying JAR..."
cp server/bin/DonCEy-Kong-Jr-Server.jar "$DIST_DIR/"

# Copy assets
echo "→ Copying assets..."
cp -r server/assets "$DIST_DIR/"

# Create launch script
echo "→ Creating launch script..."
cat > "$DIST_DIR/launch-server.sh" << 'EOF'
#!/bin/bash

# Get the directory where this script is located
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Change to that directory
cd "$DIR"

# Check if Java is installed
if ! command -v java &> /dev/null; then
    echo "ERROR: Java no está instalado."
    echo ""
    echo "Por favor instala Java 11 o superior:"
    echo ""
    echo "Ubuntu/Debian:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install openjdk-11-jre"
    echo ""
    echo "Fedora/RHEL:"
    echo "  sudo dnf install java-11-openjdk"
    echo ""
    echo "Arch:"
    echo "  sudo pacman -S jre11-openjdk"
    echo ""
    read -p "Presiona Enter para cerrar..."
    exit 1
fi

# Check Java version
JAVA_VERSION=$(java -version 2>&1 | awk -F '"' '/version/ {print $2}' | awk -F. '{sub("^1.", "", $1); print $1}')
if [ "$JAVA_VERSION" -lt 11 ]; then
    echo "ERROR: Se requiere Java 11 o superior."
    echo "Versión actual: $JAVA_VERSION"
    echo ""
    read -p "Presiona Enter para cerrar..."
    exit 1
fi

# Launch the server
echo "Iniciando DonCEy Kong Jr Server..."
echo "Presiona Ctrl+C para detener el servidor"
echo ""
java -jar DonCEy-Kong-Jr-Server.jar

# Keep terminal open on error
if [ $? -ne 0 ]; then
    echo ""
    echo "El servidor terminó con errores."
    read -p "Presiona Enter para cerrar..."
fi
EOF

chmod +x "$DIST_DIR/launch-server.sh"

# Create README
echo "→ Creating README..."
cat > "$DIST_DIR/README.txt" << 'EOF'
============================================
 DonCEy Kong Jr - Servidor Linux
============================================

REQUISITOS:
-----------
- Sistema operativo Linux (64-bit)
- Java 11 o superior (JRE)

INSTALACIÓN DE JAVA:
--------------------
Ubuntu/Debian:
  sudo apt-get update
  sudo apt-get install openjdk-11-jre

Fedora/RHEL:
  sudo dnf install java-11-openjdk

Arch:
  sudo pacman -S jre11-openjdk

CÓMO EJECUTAR:
--------------
Método 1 (Recomendado):
  ./launch-server.sh

Método 2 (Directo):
  java -jar DonCEy-Kong-Jr-Server.jar

ESTRUCTURA:
-----------
DonCEy-Kong-Jr-Server.jar  → Servidor ejecutable
launch-server.sh           → Script de lanzamiento
README.txt                 → Este archivo

CONFIGURACIÓN:
--------------
El servidor inicia automáticamente en el puerto 5000.

Para cambiar el puerto, edita src/utils/Config.java y recompila.

INTERFAZ DE ADMINISTRACIÓN:
---------------------------
El servidor incluye una interfaz gráfica (GUI) para:
• Ver jugadores conectados
• Seleccionar jugador a observar
• Generar frutas en el mapa
• Generar enemigos (cocodrilos)
• Ver eventos en tiempo real

USO:
----
1. Ejecuta el servidor (./launch-server.sh)
2. Se abrirá la interfaz gráfica
3. Los clientes pueden conectarse al puerto 5000
4. Selecciona un jugador en el panel de administración
5. Usa los botones para generar frutas/enemigos

SOLUCIÓN DE PROBLEMAS:
----------------------
• Error "Java no está instalado":
  → Instala Java 11+ (ver sección INSTALACIÓN DE JAVA)

• Error "Puerto 5000 en uso":
  → Otro proceso está usando el puerto
  → Detén el proceso conflictivo: sudo lsof -ti:5000 | xargs kill -9

• Error "Could not find or load main class Main":
  → El JAR está corrupto, vuelve a compilar

• Interfaz no aparece:
  → Verifica que tienes entorno gráfico (X11/Wayland)
  → Verifica permisos de ejecución: chmod +x launch-server.sh

• Clientes no se conectan:
  → Verifica firewall: sudo ufw allow 5000
  → Verifica que el servidor está escuchando: netstat -tulpn | grep 5000

LOGS:
-----
Los mensajes del servidor se muestran en la terminal.
Busca líneas que comiencen con [SERVER], [CLIENT], etc.

APAGAR EL SERVIDOR:
-------------------
• Desde la interfaz: Cierra la ventana
• Desde terminal: Presiona Ctrl+C
• El servidor enviará mensaje de apagado a todos los clientes

SOPORTE:
--------
Para reportar bugs o solicitar ayuda, contacta al equipo de desarrollo.

================================================
© 2024 DonCEy Kong Jr Team
================================================
EOF

echo ""
echo "✓ Linux Server build complete!"
echo "  Location: $DIST_DIR"
echo ""
echo "To test:"
echo "  cd '$DIST_DIR' && ./launch-server.sh"
echo ""
