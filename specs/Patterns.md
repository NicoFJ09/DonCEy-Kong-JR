# Design Patterns in Server-Client Architecture

Up to now, several design patterns have been implemented in establishing the server-client connection.

---

## Thread-per-Connection Pattern
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

## Data Transfer Object (DTO) Pattern
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

## Observer Pattern (Manual Implementation)
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

## Command Pattern (Future Implementation)
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

## Facade Pattern
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

## Template Method Pattern
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

## Summary

These patterns work together to create a **scalable, maintainable architecture**:

1. **Thread-per-Connection** handles concurrency
2. **DTO** separates concerns between network and game data
3. **Observer** enables real-time updates (critical for spectators)
4. **Command** will encapsulate game actions (to be implemented)
5. **Facade** simplifies complex operations
6. **Template Method** ensures consistent client lifecycle

The architecture is designed to **accommodate future game logic** without requiring major restructuring of the networking layer.