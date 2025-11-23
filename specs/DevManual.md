## Running the System

### Terminal 1: Start Server (Java) ON MACOS AND LINUX
```bash
cd server

# Limpiar
rm -rf bin/*

# Compilar
javac -d bin src/Main.java src/network/*.java src/utils/*.java src/ui/*.java src/game/*.java

# Ejecutar
java -cp bin server.src.Main
```

### Terminal 1: Start Server (Java) ON WINDOWS
```bash
cd server

# Limpiar
Remove-Item -Recurse -Force bin\* -ErrorAction SilentlyContinue

# Compilar
javac -d bin src\Main.java src\network\*.java src\utils\*.java src\ui\*.java src\game\*.java

# Ejecutar
java -cp bin server.src.Main
```

**Expected output:**
```
===========================================
🎮 DonCEy Kong Jr - Server
===========================================
Port: 12345
Max Players: 2
Spectators per Player: 2

Waiting for connections...
```

### Terminal 2+: Start Client(s) (C)
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

#### Run compiled binary

**Linux/macOS:**
```bash
./build/client
```

**Windows:**
```bash
.\build\client.exe
```

**Expected output:**
```
========================================
🎮 DonCEy Kong Jr - Client
========================================

Connecting to 127.0.0.1:12345...
✓ Connected to server!

========================================
LOBBY
========================================
...
```
