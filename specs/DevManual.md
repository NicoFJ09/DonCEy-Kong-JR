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
