# DonCEy Kong Jr - Connection Workflow

## Overview
This document describes the complete workflow from connecting a client to the server, through the lobby, to the game session.

---

## Architecture
```
┌─────────────┐                    ┌─────────────┐
│   Client    │◄──── Sockets ────► │   Server    │
│     (C)     │                    │   (Java)    │
└─────────────┘                    └─────────────┘
```

---

## Workflow Phases

### Phase 1: Connection
```
Client                          Server
  │                               │
  ├──── TCP Connect ─────────────►│
  │                               │
  │◄──── Connection Accepted ─────┤
  │                               │
```

### Phase 2: Lobby (Client Type Selection)
```
Client                          Server
  │                               │
  │◄──── Display Lobby Menu ──────┤
  │                               │
  ├──── Send Selection (1/2) ────►│
  │                               │
  │         (Server validates)    │
  │                               │
```

**Possible paths:**

#### Path A: Join as Player
```
  │◄──── ACCEPTED:PLAYER ─────────┤
  │◄──── CLIENT_ID:5 ──────────────┤
  │◄──── SESSION_START ────────────┤
  │                               │
  └──── Move to Game Session ─────►
```

#### Path B: Join as Spectator
```
  │◄──── Select Player Prompt ────┤
  │                               │
  ├──── Send Player ID ───────────►│
  │                               │
  │◄──── ACCEPTED:SPECTATOR ───────┤
  │◄──── CLIENT_ID:8 ──────────────┤
  │◄──── WATCHING:5 ───────────────┤
  │◄──── SESSION_START ────────────┤
  │                               │
  └──── Move to Game Session ─────►
```

#### Path C: Rejected (returns to lobby)
```
  │◄──── REJECTED:Players full ───┤
  │                               │
  │◄──── Display Lobby Menu ──────┤  (loops back)
```

### Phase 3: Game Session
```
Client                          Server
  │                               │
  │  (Placeholder echo loop)      │
  │                               │
  ├──── Send messages ────────────►│
  │◄──── ECHO:message ─────────────┤
  │                               │
  
  TODO: Replace with actual game logic:
  - Raylib game window
  - Send player inputs
  - Receive game state
  - Render graphics
```

---

## Message Protocol

### Client → Server

| Message | Description | Example |
|---------|-------------|---------|
| `1` | Join as player | `1` |
| `2` | Join as spectator | `2` |
| `<player_id>` | Select player to spectate | `5` |
| `exit` | Quit from lobby | `exit` |
| `back` | Cancel spectator selection | `back` |

### Server → Client

| Message | Description | Example |
|---------|-------------|---------|
| `LOBBY` | Lobby menu header | `========= LOBBY =========` |
| `ACCEPTED:PLAYER` | Player accepted | `ACCEPTED:PLAYER` |
| `ACCEPTED:SPECTATOR` | Spectator accepted | `ACCEPTED:SPECTATOR` |
| `CLIENT_ID:<id>` | Assigned client ID | `CLIENT_ID:5` |
| `WATCHING:<id>` | Spectating player ID | `WATCHING:5` |
| `SESSION_START` | Game session begins | `SESSION_START` |
| `REJECTED:<reason>` | Join rejected | `REJECTED:Players full` |
| `ERROR:<message>` | Error occurred | `ERROR:Invalid option` |
| `PLAYER_DISCONNECTED:<id>` | Watched player left | `PLAYER_DISCONNECTED:5` |
| `BYE` | Server closing connection | `BYE` |
| `ECHO:<msg>` | Echo response (placeholder) | `ECHO:hello` |

---

## User Interaction Flow

### Scenario 1: Join as Player
```
User sees:
========================================
LOBBY
========================================
Players: 0/2

Options:
  1 - Join as PLAYER
  2 - Join as SPECTATOR
  exit - Quit

> _

User types: 1 [ENTER]

Server responds:
ACCEPTED:PLAYER
CLIENT_ID:1
SESSION_START
INFO:Connected. Type 'exit' to disconnect

> _

User is now in game session (placeholder echo mode)
```

### Scenario 2: Join as Spectator
```
User sees:
========================================
LOBBY
========================================
Players: 1/2

Active players:
  Player #1 [0/2 spectators] ✓

Options:
  1 - Join as PLAYER
  2 - Join as SPECTATOR
  exit - Quit

> _

User types: 2 [ENTER]

Server responds:
Select player to spectate:
  Player #1 [0/2 spectators]

Enter player ID (or 'back'): _

User types: 1 [ENTER]

Server responds:
ACCEPTED:SPECTATOR
CLIENT_ID:2
WATCHING:1
SESSION_START
INFO:Connected. Type 'exit' to disconnect

> _

User is now spectating Player #1
```

### Scenario 3: Server Full
```
User sees:
========================================
LOBBY
========================================
Players: 2/2

Active players:
  Player #1 [2/2 spectators] ✗
  Player #2 [2/2 spectators] ✗

Options:
  1 - Join as PLAYER
  2 - Join as SPECTATOR
  exit - Quit

> _

User types: 1 [ENTER]

Server responds:
REJECTED:Players full

(Lobby menu displays again)
```

### Scenario 4: Watched Player Disconnects
```
User (Spectator) is in game session:

> _

Suddenly:
PLAYER_DISCONNECTED:1
INFO:Returning to lobby...

✓ Disconnected

(Connection closes, user must reconnect)
```

---

## Running the System

### Terminal 1: Start Server (Java)
```bash
cd server

# Limpiar
rm -rf bin/*

# Compilar
javac -d bin src/Main.java src/network/*.java src/utils/*.java

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

# Run
./build/client
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

---

## Testing Scenarios

### Test 1: Single Player
1. Start server
2. Start client 1
3. Select option `1` (Player)
4. Verify: `ACCEPTED:PLAYER`
5. Type messages in echo loop
6. Type `exit` to disconnect

### Test 2: Player + Spectator
1. Start server
2. Start client 1, select `1` (Player)
3. Start client 2, select `2` (Spectator)
4. Enter `1` to watch Player #1
5. Verify: `ACCEPTED:SPECTATOR`
6. Both clients in game session

### Test 3: Capacity Limits
1. Start server
2. Start clients 1-2, both select `1` (Player)
3. Start client 3, try to select `1`
4. Verify: `REJECTED:Players full`
5. Client 3 selects `2` (Spectator) → should work

### Test 4: Player Disconnection
1. Setup: Player #1 + Spectator watching Player #1
2. Close Player #1 (Ctrl+C or type `exit`)
3. Verify Spectator receives: `PLAYER_DISCONNECTED:1`
4. Spectator connection closes

---

## Next Steps (TODO)

- [ ] Replace `placeholderGameLoop()` in server with actual game logic
- [ ] Replace `handle_game_session()` in client with Raylib window
- [ ] Implement Command Pattern for player actions
- [ ] Implement Observer Pattern for game state broadcasting
- [ ] Add game entities (Player, Crocodiles, Fruits)
- [ ] Add collision detection
- [ ] Add score/lives system

---

## File Structure
```
project/
├── server/
│   ├── src/
│   │   ├── Main.java
│   │   ├── network/
│   │   │   ├── ClientHandler.java
│   │   │   ├── ClientType.java
│   │   │   ├── GameServer.java
│   │   │   └── Player.java
│   │   └── utils/
│   │       └── Config.java
│   └── bin/
│
├── client/
│   ├── src/
│   │   ├── main.c
│   │   ├── network/
│   │   │   ├── connection.c
│   │   │   └── connection.h
│   │   └── utils/
│   │       └── constants.h
│   ├── build/
│   └── Makefile
│
└── docs/
    └── WORKFLOW.md
```

---

## Troubleshooting

### Client can't connect
- Verify server is running
- Check SERVER_IP and SERVER_PORT in constants.h match server

### Messages not showing correctly
- Check buffer sizes (BUFFER_SIZE = 1024)
- Verify newline handling in connection_receive()

### Spectator issues
- Ensure player exists before selecting
- Check spectator limit per player (2 max)

### Compilation errors (Java)
- Make sure you're in the `server/` directory
- Package structure: `server.src.network.*` and `server.src.utils.*`
- Main class: `server.src.Main`

### Compilation errors (C)
- Make sure you're in the `client/` directory
- Check that src/network/ and src/utils/ folders exist
- Verify all .h files are in correct locations