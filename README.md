# DonCEy Kong Jr

[![Build All Platforms](https://github.com/NicoFJ09/DonCEy-Kong-JR/actions/workflows/build-all-platforms.yml/badge.svg)](https://github.com/NicoFJ09/DonCEy-Kong-JR/actions/workflows/build-all-platforms.yml)

Implementación multiplayer del clásico arcade Donkey Kong Jr con networking cliente-servidor.

## 🎮 Características

- **Cliente nativo en C** con Raylib (static linking)
- **Servidor Java** con interfaz de administración
- **Multiplayer en tiempo real** via UDP
- **Compilación automática** para Linux, Windows y macOS

## 📥 Descargar Ejecutables

### Desde GitHub Actions

1. Ve a [**Actions**](https://github.com/NicoFJ09/DonCEy-Kong-JR/actions)
2. Click en el último workflow exitoso (✓)
3. Descarga los **Artifacts** de tu plataforma:
   - 🐧 `client-linux` → Linux
   - 🪟 `client-windows` → Windows
   - 🍎 `client-macos-dmg` → macOS
   - ☕ `server-universal` → Cualquier OS con Java

### Desde Releases (cuando estén disponibles)

Ve a [**Releases**](https://github.com/NicoFJ09/DonCEy-Kong-JR/releases) para descargar versiones oficiales.

## 🚀 Inicio Rápido

### Cliente

**Linux**:
```bash
tar -xzf DonCEy-Kong-Jr-Client-Linux.tar.gz
cd DonCEy-Kong-Jr-Client-Linux
./launch-client.sh
```

**Windows**:
```cmd
Extraer DonCEy-Kong-Jr-Client-Windows.zip
Doble click en launch-client.bat
```

**macOS**:
```bash
Abrir el DMG
Arrastrar "DonCEy Kong Jr.app" a Applications
```

### Servidor

**Cualquier plataforma con Java 11+**:
```bash
# Linux/Mac
unzip DonCEy-Kong-Jr-Server-Universal.zip
./launch-server.sh

# Windows
Extraer el ZIP
Doble click en launch-server.bat
```

## 🛠️ Compilar Localmente

### Requisitos
- **Cliente**: GCC, Make, Raylib 5.5
- **Servidor**: Java JDK 11+

### Build desde tu máquina

```bash
# Cliente (tu plataforma actual)
cd client && make

# Servidor (universal)
cd server && javac -d bin src/**/*.java
```

### Build para TODAS las plataformas (GitHub Actions)

```bash
git add .
git commit -m "Update code"
git push origin main
```

Espera 10-15 minutos y los ejecutables de **Linux, Windows y macOS** estarán disponibles en Actions.

Ver [.github/GITHUB_ACTIONS.md](.github/GITHUB_ACTIONS.md) para más detalles.

## 📁 Estructura

```
client/          # Cliente C + Raylib
server/          # Servidor Java
build-tools/     # Scripts de build locales
.github/         # GitHub Actions workflows
  workflows/
    build-all-platforms.yml  # Compilación automática
```

## 🎮 Cómo Jugar

1. **Inicia el servidor** en una máquina
2. **Conecta clientes** desde otras máquinas (misma red o internet)
3. **Ingresa la IP** del servidor
4. **Juega** con controles de teclado (WASD/Flechas)

## 📝 Licencia

Proyecto académico - Instituto Tecnológico de Costa Rica

---

**Build Status**: ![Build Status](https://github.com/NicoFJ09/DonCEy-Kong-JR/actions/workflows/build-all-platforms.yml/badge.svg)
