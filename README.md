# DonCEy Kong Jr

Implementación multiplayer del clásico arcade Donkey Kong Jr con networking cliente-servidor.

## 🎮 Características

- **Cliente nativo en C** con Raylib (static linking)
- **Servidor Java** con interfaz de administración
- **Multiplayer en tiempo real** via UDP
- **Compilación local** para Linux, Windows y macOS

## 📥 Obtener Ejecutables

Para obtener ejecutables, debes compilar localmente en tu plataforma. Ver la sección **🛠️ Compilar Localmente** más abajo.

## 🚀 Inicio Rápido

### Cliente

Después de compilar (ver sección siguiente):

**Linux/macOS**:
```bash
cd client/build
./client
```

**Windows**:
```cmd
cd client\build
client.exe
```

### Servidor

Después de compilar:

```bash
# Linux/Mac
cd server/bin
java -jar DonCEy-Kong-Jr-Server.jar

# Windows
cd server\bin
java -jar DonCEy-Kong-Jr-Server.jar
```

## 🛠️ Compilar Localmente

### Requisitos
- **Cliente**: GCC, Make, Raylib 5.5 (static library)
- **Servidor**: Java JDK 11+

### Cliente (Makefile multiplataforma)

```bash
cd client
make           # Compilar
make verify    # Verificar que es standalone (sin dependencias externas)
make run       # Compilar y ejecutar
make clean     # Limpiar build
```

El Makefile detecta automáticamente tu plataforma (Linux/macOS/Windows) y configura el linking correcto.

### Servidor (Java)

Usa los scripts de build incluidos:

**Linux**:
```bash
./build-tools/build-server-linux.sh
```

**macOS**:
```bash
./build-tools/build-server-macos.sh
```

**Windows**:
```cmd
build-tools\build-server-windows.bat
```

Estos scripts generan distribuciones completas con ejecutable JAR, scripts de lanzamiento y README.

## 📁 Estructura

```
client/          # Cliente C + Raylib
  src/           # Código fuente
  assets/        # Sprites, fuentes, iconos
  Makefile       # Sistema de build multiplataforma
  client.rc      # Recurso para icono de Windows
server/          # Servidor Java
  src/           # Código fuente
  assets/        # Recursos del servidor
build-tools/     # Scripts de build para distribución
```

## 🎮 Cómo Jugar

1. **Inicia el servidor** en una máquina
2. **Conecta clientes** desde otras máquinas (misma red o internet)
3. **Ingresa la IP** del servidor
4. **Juega** con controles de teclado (WASD/Flechas)

## 📝 Licencia

Proyecto académico - Instituto Tecnológico de Costa Rica
