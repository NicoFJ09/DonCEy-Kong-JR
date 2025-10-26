# Task Distribution - DonCEy Kong Jr (4 People)

## 🎯 Core Principle: Minimal Cross-Dependencies

Each person owns a **vertical slice** that can be developed and tested independently. Integration happens at defined **interface boundaries**.

---

## 👥 Team Roles & Task Assignment

### **Person A: Tamara**
### **Role: Game State & Entity System (Server Core)**

**Responsibilities:**

#### **1. Game Entities Package** (`server/src/game/entities/`)
```
Entity.java (abstract base)
  ├── Position.java
  ├── GamePlayer.java
  │     ├── position, velocity, lives, score
  │     ├── climbing state, facing direction
  │     └── movement methods
  ├── Crocodile.java (abstract)
  │     ├── RedCrocodile.java (vertical patrol)
  │     └── BlueCrocodile.java (falling)
  └── Fruit.java
        ├── position, points
        └── collected state
```

#### **2. Game State Manager** (`server/src/game/state/`)
```
GameState.java
  ├── List of players, crocodiles, fruits
  ├── Level number, difficulty multiplier
  ├── Add/remove entity methods
  ├── Get entity by ID
  └── Serialize state to JSON/String (for network)
```

#### **3. Collision Detection** (`server/src/game/managers/`)
```
CollisionManager.java
  ├── checkPlayerCrocodileCollision()
  ├── checkPlayerFruitCollision()
  ├── checkPlayerFallDeath()
  └── Returns collision events (List<CollisionEvent>)
```

**Deliverable Interface:**
```java
// What others need from you:
public class GameState {
    public void addPlayer(Integer clientId, Position startPos);
    public void removePlayer(Integer clientId);
    public void addCrocodile(CrocodileType type, Integer vineId);
    public void addFruit(Integer vineId, Float height, Integer points);
    public void updatePlayerPosition(Integer clientId, Position newPos);
    public String serializeState(); // For network broadcast
    public List<CollisionEvent> getCollisions(); // For game loop
}
```

**Dependencies:** NONE - Pure game logic, no networking

**Testing:** Write main() methods to test entity creation and collision without server

---

### **Person B: David**
### **Role: Game Loop & Command System (Server Integration)**

**Responsibilities:**

#### **1. Command Pattern** (`server/src/game/commands/`)
```
Command.java (interface)
  ├── execute(GameState state, Integer playerId)
  
MoveCommand.java      // UP, DOWN, LEFT, RIGHT
JumpCommand.java      // Space bar
ClimbCommand.java     // Grab vine
```

#### **2. Game Loop** (`server/src/game/GameLoop.java`)
```
GameLoop (Thread)
  ├── Runs at fixed tick rate (60 FPS)
  ├── Update all entity positions
  ├── Check collisions (call CollisionManager)
  ├── Handle collision events (death, fruit collection)
  ├── Broadcast state to all clients
  └── Handle level completion
```

#### **3. Integration with ClientHandler**
```
Modify ClientHandler.handlePlayerSession():
  ├── Parse incoming commands (MOVE:UP, JUMP, etc.)
  ├── Create Command objects
  ├── Add to GameLoop command queue
  
Modify ClientHandler.handleSpectatorSession():
  ├── Subscribe to GameLoop state broadcasts
  ├── Forward state to spectator client
```

**Deliverable Interface:**
```java
// What you expose:
public class GameLoop extends Thread {
    public void queueCommand(Command cmd, Integer playerId);
    public void registerStateListener(ClientHandler listener);
    public void start(); // Starts game loop thread
}

// Protocol format you define:
"STATE:<json>"
"PLAYER_DIED:<playerId>"
"SCORE:<playerId>:<score>"
```

**Dependencies:** 
- Needs Person A's `GameState` and entities (interface defined above)
- You define the protocol format

**Testing:** Create mock ClientHandlers, send commands, verify state updates

---

### **Person C: Fabiola**
### **Role: Admin Console & Entity Factory (Server Management)**

**Responsibilities:**

#### **1. Admin Console** (`server/src/admin/`)
```
AdminConsole.java
  ├── Separate thread reading System.in
  ├── Parse admin commands
  ├── Call EntityManager
  └── Display status
  
Commands:
  create red <vineId>
  create blue <vineId>
  create fruit <vineId> <height> <points>
  delete fruit <vineId> <height>
  list entities
  status
  exit
```

#### **2. Entity Manager** (`server/src/game/managers/`)
```
EntityManager.java
  ├── createRedCrocodile(vineId)
  ├── createBlueCrocodile(vineId)
  ├── createFruit(vineId, height, points)
  ├── deleteFruit(vineId, height)
  ├── Validates positions (vine exists, height valid)
  └── Interacts with GameState to add/remove
```

#### **3. Factory Pattern Implementation**
```
EntityFactory.java
  ├── createCrocodile(type, vineId) -> Crocodile
  ├── createFruit(vineId, height, points) -> Fruit
  └── Handles entity ID generation
```

**Deliverable Interface:**
```java
// What you expose:
public class AdminConsole extends Thread {
    public AdminConsole(EntityManager manager);
    public void start(); // Starts admin input loop
}

public class EntityManager {
    public boolean createCrocodile(CrocodileType type, Integer vineId);
    public boolean createFruit(Integer vineId, Float height, Integer points);
    public boolean deleteFruit(Integer vineId, Float height);
}
```

**Dependencies:** 
- Needs Person A's `GameState` reference
- Needs Person A's entity classes

**Testing:** Run AdminConsole standalone with mock GameState, test all commands

---

### **Person D: Nicolás**
### **Role: Client Graphics & Protocol Bridge (C + Integration)**

**Responsibilities:**

#### **1. Client Game Structs** (`client/src/game/`)
```c
// entities.h
typedef struct {
    float x, y;
    float velocity_x, velocity_y;
    int lives, score;
    bool is_climbing;
} Player;

typedef struct {
    int id;
    float x, y;
    int vine_id;
    bool is_red; // true = red, false = blue
} Crocodile;

typedef struct {
    int id;
    float x, y;
    int points;
} Fruit;

typedef struct {
    Player players[2];
    Crocodile crocodiles[MAX_ENEMIES];
    Fruit fruits[MAX_FRUITS];
    int num_crocodiles, num_fruits;
} GameState;
```

#### **2. Protocol Parser** (`client/src/game/state_parser.c`)
```c
// Parse server messages into structs
bool parse_game_state(char* msg, GameState* state);
bool parse_player_died(char* msg, int* player_id);
bool parse_score_update(char* msg, int* player_id, int* score);
```

#### **3. Raylib Renderer** (`client/src/rendering/`)
```c
// renderer.h/c
void init_renderer();
void render_game_state(GameState* state);
void render_player(Player* player);
void render_crocodile(Crocodile* croc);
void render_fruit(Fruit* fruit);
void render_ui(int lives, int score);
void close_renderer();
```

#### **4. Input Handler** (`client/src/input/`)
```c
// input_handler.h/c
typedef enum {
    INPUT_NONE,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_JUMP,
    INPUT_QUIT
} InputAction;

InputAction get_player_input();
char* input_to_command(InputAction action); // "MOVE:UP", "JUMP"
```

#### **5. Integration in main.c**
```c
// Replace handle_game_session() with:
void handle_game_session() {
    init_renderer();
    GameState state = {0};
    
    while (connected) {
        // Check socket for state updates
        if (has_server_data()) {
            char* msg = receive();
            parse_game_state(msg, &state);
        }
        
        // Check keyboard input
        InputAction input = get_player_input();
        if (input != INPUT_NONE) {
            send_command(input_to_command(input));
        }
        
        // Render
        render_game_state(&state);
    }
    
    close_renderer();
}
```

**Deliverable Interface:**
```c
// Protocol format you define (coordinate with Person B):
// Server → Client
"STATE:{json}" or custom format
"PLAYER_DIED:1"
"SCORE:1:500"

// Client → Server  
"MOVE:UP"
"MOVE:DOWN"
"MOVE:LEFT"
"MOVE:RIGHT"
"JUMP"
```

**Dependencies:** 
- Protocol format from Person B
- **NO dependency on their implementation** - use mock data during dev

**Testing:** Create fake game state, render it without server connection

---

## 📋 Development Phases (Parallel Work)

### **Phase 1: Independent Development (Week 1-2)**

Everyone works on their module **completely independently**:

**Person A:**
- Create all entity classes
- Write unit tests (main methods)
- Document entity interfaces
- ✅ Deliverable: Entities package compiles, GameState works standalone

**Person B:**
- Create Command pattern structure
- Create GameLoop skeleton (empty update logic)
- Define protocol format document
- ✅ Deliverable: Commands can be created, GameLoop thread runs

**Person C:**
- Create AdminConsole with all command parsing
- Create EntityManager with validation
- Test with mock GameState
- ✅ Deliverable: Admin console works with fake data

**Person D (You):**
- Set up Raylib in Makefile
- Create all structs
- Create renderer that displays fake game state
- Create input handler
- ✅ Deliverable: Game window shows moving sprites (mock data)

---

### **Phase 2: Interface Integration (Week 2)**

**Integration Points (You coordinate):**

#### **Integration 1: GameState → GameLoop**
```java
// Person B calls Person A's code:
GameState state = new GameState();
List<CollisionEvent> collisions = collisionManager.check(state);
String stateJson = state.serializeState();
```

#### **Integration 2: AdminConsole → GameState**
```java
// Person C calls Person A's code:
entityManager.createCrocodile(RED, vineId);
// EntityManager calls GameState.addCrocodile()
```

#### **Integration 3: GameLoop → ClientHandler**
```java
// Person B modifies existing ClientHandler:
// Parse "MOVE:UP" → new MoveCommand(UP)
// gameLoop.queueCommand(cmd, playerId);
```

#### **Integration 4: Server Protocol → Client Parser**
```c
// You (Person D) parse Person B's protocol:
if (strncmp(buffer, "STATE:", 6) == 0) {
    parse_game_state(buffer + 6, &state);
}
```

---

### **Phase 3: End-to-End Testing (Week 3)**

Everyone together:
- Server + Client full integration
- Test all features
- Fix bugs
- Polish & documentation

---

## 🔧 Key Interface Contracts (Define First!)

Before anyone starts coding, **YOU** define these contracts:

### **1. GameState Interface** (Person A must implement)
```java
public class GameState {
    // REQUIRED methods for others:
    public void addPlayer(Integer id, Position pos);
    public void updatePlayer(Integer id, Position pos);
    public void addCrocodile(CrocodileType type, Integer vineId);
    public void addFruit(Integer vineId, Float height, Integer points);
    public String serializeState(); // JSON format
}
```

### **2. Protocol Format** (Person B defines, Person D implements)
```
// Server → Client
STATE:<json_object>
PLAYER_DIED:<player_id>
SCORE:<player_id>:<score>
LEVEL_COMPLETE:<player_id>

// Client → Server
MOVE:UP|DOWN|LEFT|RIGHT
JUMP
CLIMB
```

### **3. Command Interface** (Person B implements)
```java
public interface Command {
    void execute(GameState state, Integer playerId);
}
```

### **4. EntityManager Interface** (Person C implements)
```java
public class EntityManager {
    public boolean createCrocodile(CrocodileType type, Integer vineId);
    public boolean createFruit(Integer vineId, Float height, Integer points);
}
```

---

## 📊 Dependency Graph

```
Person A (Entities)
    ↓ provides classes
Person C (Admin) ──→ Creates entities
    ↓
Person B (GameLoop) ──→ Updates entities, broadcasts state
    ↓ sends protocol
Person D (Client) ──→ Renders state
```

**Critical:** Person A finishes entities first, then others can start integration.

---

## ✅ Success Criteria Per Person

**Person A:**
- [ ] All entity classes compile
- [ ] GameState can add/remove entities
- [ ] CollisionManager detects collisions
- [ ] Can serialize GameState to String
- [ ] Unit tests pass

**Person B:**
- [ ] GameLoop runs at 60 FPS
- [ ] Commands execute on GameState
- [ ] State broadcasts to clients
- [ ] Protocol format documented
- [ ] Integration with ClientHandler works

**Person C:**
- [ ] Admin console accepts all commands
- [ ] EntityManager validates input
- [ ] Entities created through admin appear in game
- [ ] Help documentation written
- [ ] Factory pattern implemented

**Person D:**
- [ ] Raylib window displays
- [ ] Keyboard input detected
- [ ] Game state parsed from protocol
- [ ] All entities render correctly
- [ ] Smooth animations
- [ ] Protocol bridge works both ways

---

## 🚀 Who Starts First?

**Week 1 Priority Order:**

1. **Person A** - Start immediately (everyone waits for entities)
2. **Person D (You)** - Start immediately (independent, can use mock data)
3. **Person B** - Start structure, wait for Person A's interfaces
4. **Person C** - Start structure, wait for Person A's interfaces

---

## 💬 Communication Points

**Daily Sync (5 min):**
- Person A: "Entity interfaces ready? ETA?"
- Person B: "Protocol format finalized?"
- Person C: "Admin commands defined?"
- Person D: "Client can parse protocol?"

**Week 1 Milestone:** Everyone shows their module working standalone

**Week 2 Milestone:** Integration points work (tested pairwise)

**Week 3 Milestone:** Full game playable

---