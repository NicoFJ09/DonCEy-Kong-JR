# Manual de Usuario - DonCEy Kong Jr
DonCEy Kong Jr., es un juego basado en la lógica del clásico Donkey Kong Jr., cuenta con una arquitectura cliente(C)-servidor(Java) que permite interacción de jugadores y espectadores en tiempo real.

## Requisitos del Sistema

**Requisitos de Software:**
- Java Development Kit (JDK) 11 o superior: Necesario para compilar y ejecutar el servidor.
- GCC (GNU Compiler Collection) o cualquier compilador compatible con C: Necesario para compilar el cliente.
- Make: Herramienta de automatización de compilación para facilitar la construcción del cliente.
- Raylib: Biblioteca gráfica utilizada por el cliente para renderizar gráficos y manejar entradas.
- Conexión de red local o acceso a Internet: Para permitir la comunicación entre el cliente y el servidor.

**Requisitos de Hardware:**
- Teclado funcional para controlar el juego.
- 300 MB de espacio libre en disco para la instalación de requisitos citados de software
- Al menos 2 GB de RAM para un rendimiento óptimo durante el juego.

## Ejecución del Juego

### Terminal 1: Ejecución del Servidor (Java) EN MACOS Y LINUX
```bash
cd server

# Limpiar
rm -rf bin/*

# Compilar
javac -d bin src/Main.java src/network/*.java src/utils/*.java src/ui/*.java src/game/*.java src/gamestate/GameSession.java

# Ejecutar
java -cp bin server.src.Main

# IMPORTANTE: Solo se permite una instancia del servidor a la vez
# Si intentas ejecutar una segunda instancia, verás un mensaje de error
# y la aplicación no se iniciará
```

### Terminal 1: Ejecución del Servidor (Java) EN WINDOWS
```bash
cd server

# Limpiar
Remove-Item -Recurse -Force bin\* -ErrorAction SilentlyContinue

# Compilar
javac -d bin src\Main.java src\network\*.java src\utils\*.java src\ui\*.java src\game\*.java src\gamestate\GameSession.java

# Ejecutar
java -cp bin server.src.Main
```

**Salida esperada:**

1) En terminal del Servidor:
```
===========================================
 DonCEy Kong Jr - Server
===========================================
Port: 12345
Max Players: 2
Spectators per Player: 2

Waiting for connections...
```
2) Pantalla de Administrador del Servidor.

### Terminal 2+: Ejecución del Cliente(s) (C)
```bash
cd client

# Compilar
make

# Limpiar
make clean

# Compilar y ejecutar
make run

# Recompilar desde cero
make rebuild
```

#### Correr el binario del cliente directamente

**Linux/macOS:**
```bash
./build/client
```

**Windows:**
```bash
.\build\client.exe
```

**Salida esperada:**

1) En terminal del Cliente:
```
========================================
 DonCEy Kong Jr - Cliente
========================================

Connecting to 127.0.0.1:12345...
✓ Connected to server!

========================================
LOBBY
========================================
...
```
2) Ventana del juego del Cliente.

---

## Distribución de Ejecutables

Para crear ejecutables standalone para distribución en diferentes plataformas, el proyecto incluye scripts de build automatizados en el directorio `build-tools/`.

### Build Rápido (Plataforma Actual)

Desde la raíz del proyecto:

```bash
./build-tools/build-all.sh
```

Este script detectará automáticamente tu sistema operativo y compilará:
- Cliente nativo para tu plataforma
- Servidor (JAR + launcher) para tu plataforma

Los ejecutables se generarán en: `dist/[plataforma]/`

### Builds Individuales

Puedes compilar componentes específicos para cada plataforma:

#### Cliente macOS (.app bundle)
```bash
./build-tools/build-client-macos.sh
```
Crea: `dist/macos/DonCEy Kong Jr.app` (doble-click para ejecutar)
También genera: `dist/macos/DonCEy-Kong-Jr-Client-macOS.dmg` (opcional)

#### Cliente Linux (binario + assets)
```bash
./build-tools/build-client-linux.sh
```
Crea: `dist/linux/DonCEy-Kong-Jr-Client/` con binario y launcher script
Ejecutar: `./launch.sh`

#### Cliente Windows (cross-compilación)
```bash
./build-tools/build-client-windows.sh
```
**Requisitos previos**:
- mingw-w64 toolchain: `brew install mingw-w64` (macOS) o `apt-get install mingw-w64` (Linux)
- Raylib Windows libraries

Crea: `dist/windows/DonCEy-Kong-Jr-Client/DonCEy-Kong-Jr.exe`

**Nota**: Este script hace cross-compilación desde macOS/Linux. Para agregar icono al .exe:
```bash
npm install -g rcedit
rcedit dist/windows/DonCEy-Kong-Jr-Client/DonCEy-Kong-Jr.exe --set-icon client/assets/icons/windows/icon.ico
```

#### Servidor macOS (.app bundle con JAR)
```bash
./build-tools/build-server-macos.sh
```
Crea: `dist/macos/DonCEy Kong Jr Server.app`
También genera: `dist/macos/DonCEy-Kong-Jr-Server-macOS.dmg`
**Requiere**: Java 11+ instalado en el sistema

#### Servidor Linux (JAR + launcher)
```bash
./build-tools/build-server-linux.sh
```
Crea: `dist/linux/DonCEy-Kong-Jr-Server/` con JAR y script de lanzamiento
Ejecutar: `./launch-server.sh`
**Requiere**: Java 11+ instalado en el sistema

#### Servidor Windows (JAR + batch launcher)
```bash
./build-tools/build-server-windows.bat
```
Crea: `dist/windows/DonCEy-Kong-Jr-Server/` con JAR y Launch-Server.bat
Ejecutar: Doble-click en `Launch-Server.bat`
**Requiere**: Java 11+ instalado en el sistema

### Estructura de Distribución

Después de ejecutar los builds, la estructura será:

```
dist/
├── macos/
│   ├── DonCEy Kong Jr.app              (Cliente)
│   ├── DonCEy Kong Jr Server.app       (Servidor)
│   ├── DonCEy-Kong-Jr-Client-macOS.dmg (Instalador cliente)
│   └── DonCEy-Kong-Jr-Server-macOS.dmg (Instalador servidor)
├── linux/
│   ├── DonCEy-Kong-Jr-Client/
│   │   ├── DonCEy-Kong-Jr              (Ejecutable)
│   │   ├── launch.sh                   (Launcher)
│   │   ├── assets/                     (Recursos)
│   │   └── README.txt
│   └── DonCEy-Kong-Jr-Server/
│       ├── DonCEy-Kong-Jr-Server.jar
│       ├── launch-server.sh
│       └── README.txt
└── windows/
    ├── DonCEy-Kong-Jr-Client/
    │   ├── DonCEy-Kong-Jr.exe
    │   ├── assets/
    │   └── README.txt
    └── DonCEy-Kong-Jr-Server/
        ├── DonCEy-Kong-Jr-Server.jar
        ├── Launch-Server.bat
        └── README.txt
```

### Notas de Distribución

**Cliente**:
- **macOS**: El .app bundle es autocontenido y portable. Usuarios pueden copiarlo a Aplicaciones.
- **Linux**: Requiere bibliotecas del sistema (OpenGL, X11, etc.). Ver README.txt en la distribución para detalles.
- **Windows**: El .exe es portable pero puede requerir Visual C++ Redistributable.

**Servidor**:
- Todas las plataformas requieren Java 11 o superior instalado.
- El servidor inicia en el puerto 5000 por defecto.
- Los launchers verifican la versión de Java automáticamente.
- La interfaz gráfica de administración se abre automáticamente al ejecutar.

**Assets**:
- Los assets del cliente están embebidos en cada distribución (carpeta `assets/` copiada).
- Total ~50MB de sprites, fuentes y recursos.
- No es necesario descargar assets por separado.

### Solución de Problemas de Distribución

**"Permission denied" al ejecutar scripts**:
```bash
chmod +x build-tools/*.sh
```

**mingw-w64 no encontrado (Windows cross-compilation)**:
- macOS: `brew install mingw-w64`
- Linux: `sudo apt-get install mingw-w64` o equivalente

**Servidor no inicia (error de Java)**:
- Verifica versión: `java -version` (debe ser 11+)
- Instala OpenJDK: https://adoptium.net

**Cliente Linux no encuentra bibliotecas**:
```bash
# Ubuntu/Debian
sudo apt-get install libgl1 libx11-6 libxrandr2 libxi6

# Fedora
sudo dnf install mesa-libGL libX11 libXrandr libXi
```

### Iconos

Los iconos para cada plataforma están en:
- **macOS**: `client/assets/icons/macos/client.icns` y `server.icns`
- **Windows**: `client/assets/icons/windows/client.ico` y `server.ico`
- **Linux**: `client/assets/icons/linux/client.png` y `server.png`

Los scripts de build los aplican automáticamente durante la compilación
