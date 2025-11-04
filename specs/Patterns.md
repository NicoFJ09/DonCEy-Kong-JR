# Design Patterns in Server-Client Architecture

Up to now, several design patterns have been implemented in establishing the server-client connection.

---

## Server Patterns

### Thread-per-Connection Pattern
```java
ClientHandler handler = new ClientHandler(socket, clientId, clientAddress, this);
handler.start(); // Each ClientHandler is a Thread
```
**Characteristics:**
- **Inheritance:** `ClientHandler extends Thread`
- **Polymorphism:** Override of `run()` method
- **Encapsulation:** Each handler manages its own client independently
- **Purpose:** Enables multiple simultaneous client connections without blocking

---

### Data Transfer Object (DTO) Pattern
```java
public class NetworkPlayer {
    private Integer id;
    private String address;
    private List<Integer> spectatorIds;
    // + getters/setters
}
```
**Characteristics:**
- Encapsulates network-related player data
- Separates connection metadata from game logic
- Provides controlled access through getters/setters
- **Future separation:** Will coexist with `GamePlayer` class for game state

---

### Observer Pattern (Manual Implementation)
```java
// When a player disconnects, notify observers (spectators)
for (Integer spectatorId : orphanedSpectators) {
    ClientHandler spectatorHandler = clientHandlers.get(spectatorId);
    if (spectatorHandler != null) {
        spectatorHandler.notifyPlayerDisconnected(id); // Observer notification
    }
}
```
**Current Implementation:**
- **Subject:** Player being observed
- **Observers:** Spectators watching that player
- **Notification:** `notifyPlayerDisconnected()` method
- Limited to disconnect notifications for now

**Future Extensions:**
- Broadcasting game state updates to spectators
- Real-time position, score, and action updates
- Event notifications (collisions, power-ups, game over)
- Will become the primary pattern for spectator functionality

---

### Command Pattern (Future Implementation)
```java
// To be implemented for game actions
interface Command {
    void execute(GameState gameState, Integer playerId);
}

class MoveCommand implements Command {
    private float dx, dy;
    public void execute(GameState state, Integer playerId) {
        state.getPlayer(playerId).move(dx, dy);
    }
}
```
**Purpose:**
- Encapsulate player actions (move, jump, climb)
- Enable action queuing and validation
- Support replay and undo functionality
- Decouple input handling from game logic

---

### Facade Pattern
```java
public class GameServer {
    public void start() { /* Hides socket complexity */ }
    public boolean registerPlayer(...) { /* Hides synchronization */ }
    public void cleanup(...) { /* Hides thread management */ }
}
```
**Purpose:**
- Simplifies complex networking operations
- Provides clean interface for server management
- Hides thread synchronization and socket handling
- Reduces coupling between network and game layers

---

### Template Method Pattern
```java
@Override
public void run() {
    // 1. Setup (always the same)
    input = new BufferedReader(...);
    output = new PrintWriter(...);
    
    // 2. Client type selection (always the same)
    type = selectClientType();
    
    // 3. Handle session (varies by type)
    if (type == ClientType.PLAYER) {
        handlePlayerSession();    // Specific implementation
    } else {
        handleSpectatorSession(); // Specific implementation
    }
    
    // 4. Cleanup (always the same)
    finally {
        close();
        server.cleanup(id);
    }
}
```
**Characteristics:**
- Defines skeleton of client connection lifecycle
- Delegates type-specific behavior to submethod implementations
- Ensures consistent setup and cleanup across all clients
- Allows behavior variation without changing overall structure

---

## Client Patterns

### Strategy Pattern
```c
// main.c
bool accepted = lobby_handle(conn);  // Strategy 1: Lobby phase
if (accepted && conn->connected) {
    session_handle(conn);            // Strategy 2: Game session phase
}
```
**Characteristics:**
- `lobby_handle()` and `session_handle()` are interchangeable strategies
- Each strategy encapsulates a specific phase of client behavior
- Main flow delegates to appropriate strategy based on state
- Easy to replace strategies without modifying main flow

**Purpose:**
- Separate lobby logic from game session logic
- Enable independent development and testing of each phase
- Allow complete replacement of session handler (e.g., text → Raylib)

---

### State Pattern (via MessageType Enum)
```c
typedef enum {
    MSG_TYPE_ACCEPTED,
    MSG_TYPE_REJECTED,
    MSG_TYPE_SESSION_START,
    MSG_TYPE_PLAYER_DISCONNECTED,
    // ...
} MessageType;

MessageType type = message_parse(buffer, &parsed);
switch (type) {
    case MSG_TYPE_ACCEPTED: /* handle acceptance */ break;
    case MSG_TYPE_REJECTED: /* handle rejection */ break;
    // ...
}
```
**Characteristics:**
- Protocol messages represented as distinct states
- Centralized parsing logic determines message type
- Behavior varies based on current message state
- Easy to add new message types without breaking existing code

**Purpose:**
- Type-safe message handling
- Prevents string comparison errors
- Provides compile-time safety for protocol handling
- Simplifies debugging (can trace state transitions)

---

### Facade Pattern (Client-Side)
```c
// lobby_handler.h - Simple interface
bool lobby_handle(Connection* conn);

// session_handler.h - Simple interface  
void session_handle(Connection* conn);

// message_handler.h - Simple interface
MessageType message_parse(const char* buffer, ParsedMessage* parsed);
void message_display(const char* buffer, bool is_prompt);
```
**Purpose:**
- Hide complex lobby interaction logic behind `lobby_handle()`
- Hide complex session logic behind `session_handle()`
- Hide protocol parsing complexity behind `message_parse()`
- Main flow only needs to call high-level functions

---

### Separation of Concerns (Modular Architecture)
```
network/          → Low-level socket operations
  ├── connection.c       (TCP send/receive)
  ├── message_handler.c  (Protocol parsing)
  ├── lobby_handler.c    (Lobby phase logic)
  └── session_handler.c  (Game phase logic)
```
**Characteristics:**
- Each module has single, clear responsibility
- Minimal coupling between modules
- Easy to test each module independently
- Clear extension points for new features

**Benefits:**
- **Network layer:** Can be reused for other projects
- **Message handler:** Easy to add new protocol types
- **Lobby handler:** Can modify lobby UI without touching game code
- **Session handler:** Can completely replace with Raylib without affecting other modules

---

## Summary

These patterns work together to create a **scalable, maintainable architecture**:

### Server-Side:
1. **Thread-per-Connection** handles concurrency
2. **DTO** separates concerns between network and game data
3. **Observer** enables real-time updates (critical for spectators)
4. **Command** will encapsulate game actions (to be implemented)
5. **Facade** simplifies complex operations
6. **Template Method** ensures consistent client lifecycle

### Client-Side:
1. **Strategy** separates lobby and game session phases
2. **State** provides type-safe protocol handling via enums
3. **Facade** hides complexity behind simple interfaces
4. **Separation of Concerns** creates modular, testable components

The architecture is designed to **accommodate future game logic** without requiring major restructuring of the networking layer. Both client and server can be extended independently through clearly defined extension points.

---

## Key Design Decisions

**Why separate handlers?**
- Lobby logic is fundamentally different from game logic
- Allows parallel development by different team members
- Makes testing easier (mock each phase independently)

**Why use message parsing layer?**
- Centralizes protocol knowledge in one place
- Type-safe message handling prevents bugs
- Easy to extend with new message types
- Consistent error handling

**Why keep network layer separate?**
- Core socket operations rarely change
- Can be reused across different projects
- Testing network layer doesn't require game logic