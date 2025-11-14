# DonCEy Kong Jr - Project Analysis & Team Meeting Preparation

## 🎮 Game Requirements Analysis

### Core Gameplay Elements

1. **Player Character**: Donkey Kong Jr
   - Movement: Arrow keys (up/down/left/right)
   - Actions: Jump (to grab vines)
   - Lives system
   - Death conditions: Hit by crocodile OR fall to abyss

2. **Enemies**: 
   - **Red Crocodiles**: Move up/down on single vine (stay on vine)
   - **Blue Crocodiles**: Fall vertically down vines
   - Speed increases with each level completion

3. **Collectibles**: Fruits
   - Give points when collected
   - Admin-created with position (vine, height) and point value

4. **Level Progression**:
   - Rescue Donkey Kong = +1 life & restart level
   - Enemy speed increases each completion

5. **Admin Controls** (Console/App):
   - Create crocodiles (type, vine position)
   - Create fruits (vine, height, points)
   - Delete fruits

### System Architecture Requirements

1. **Max Concurrent Users**:
   - 2 Players (different machines, not same screen)
   - 2 Spectators per Player (4 total spectators max)

2. **Required Design Patterns** (Java):
   - At least 2 patterns (excluding Singleton)
   - Current: Observer, Thread-per-Connection, DTO, Template Method
   - Needed: Command, Factory

3. **C Requirements**:
   - Constants in separate file ✅
   - Use of structs ✅
   - Executable required

---

## 🔌 Current Implementation Status

### ✅ COMPLETED: Network Layer

#### Server (Java)
```
✅ GameServer.java      - Main server, accepts connections
✅ ClientHandler.java   - Thread per client
✅ NetworkPlayer.java   - Player connection metadata
✅ Config.java          - Configuration constants
```

**Key Features Implemented**:
- Multi-client support via threads
- Player vs Spectator role selection
- Spectator assignment to specific players
- Disconnect handling with observer notification
- Thread-safe operations (ConcurrentHashMap)

#### Client (C)
```
✅ connection.c/h       - TCP socket operations
✅ lobby_handler.c/h    - Lobby phase logic
✅ session_handler.c/h  - Game session placeholder
✅ message_handler.c/h  - Protocol parsing
✅ constants.h          - Configuration
```

**Key Features Implemented**:
- Cross-platform socket code (Windows/Linux/macOS)
- Clean separation of lobby vs game phases
- Type-safe message parsing
- Modular architecture

### 🚧 TODO: Game Logic Implementation

#### Server Needs:
1. **Game Entities** (classes for Player, Crocodiles, Fruits)
2. **Game State Manager** (positions, collisions, score)
3. **Game Loop** (60 FPS update cycle)
4. **Command Pattern** (handle player inputs)
5. **Admin Console** (entity creation/deletion)
6. **Factory Pattern** (entity creation)

#### Client Needs:
1. **Raylib Integration** (graphics rendering)
2. **Game State Structures** (mirror server state)
3. **Input Handler** (keyboard → commands)
4. **State Parser** (protocol → structs)
5. **Renderer** (draw entities, UI)

---

## 🔄 Proposed Game Flow

### 1. Connection Phase (✅ DONE)
```
Client connects → Server accepts → ClientHandler thread spawned
```

### 2. Lobby Phase (✅ DONE)
```
Client chooses:
├─ PLAYER → If slots available → Enter game
├─ SPECTATOR → Select player → Watch game
└─ EXIT → Disconnect
```

### 3. Game Initialization (🚧 TODO)
```
Server:
├─ Create GameState
├─ Spawn initial entities
├─ Start GameLoop thread
└─ Broadcast initial state

Client:
├─ Initialize Raylib window
├─ Parse initial state
└─ Start render loop
```

### 4. Main Game Loop (🚧 TODO)
```
Every frame (60 FPS):
Server:
├─ Process command queue
├─ Update entity positions
├─ Check collisions
├─ Handle deaths/scoring
└─ Broadcast state to all clients

Client:
├─ Poll keyboard input
├─ Send commands to server
├─ Receive state updates
└─ Render game state
```

### 5. Admin Operations (🚧 TODO)
```
Admin Console (parallel thread):
├─ Read console commands
├─ Validate parameters
├─ Create/delete entities
└─ Update GameState
```

---

## 📨 Protocol Design (Proposed)

### Client → Server Commands
```
MOVE:UP
MOVE:DOWN  
MOVE:LEFT
MOVE:RIGHT
JUMP
CLIMB
```

### Server → Client Messages
```
STATE:<json>                    # Full game state update
PLAYER_DIED:<id>               # Player death notification
SCORE_UPDATE:<id>:<score>      # Score change
FRUIT_COLLECTED:<id>:<points>  # Fruit collection
LEVEL_COMPLETE                 # Level completed
GAME_OVER                       # Game ended
```

### State JSON Format (Proposed)
```json
{
  "players": [
    {"id": 1, "x": 100, "y": 200, "lives": 3, "score": 500, "climbing": false}
  ],
  "crocodiles": [
    {"id": 1, "type": "red", "vine": 2, "y": 150, "direction": "up"},
    {"id": 2, "type": "blue", "vine": 4, "y": 50}
  ],
  "fruits": [
    {"id": 1, "vine": 3, "height": 0.5, "points": 100}
  ],
  "level": 1,
  "speed_multiplier": 1.0
}
```

---

## 👥 Team Task Allocation (From Tasks.md)

### Person A (Tamara) - Game Core
- Entity classes (Player, Crocodile, Fruit)
- GameState manager
- Collision detection
- **Deliverable**: Standalone game logic module

### Person B (David) - Game Loop & Commands
- Command pattern implementation
- Game loop (60 FPS)
- ClientHandler integration
- State broadcasting
- **Deliverable**: Command processing & state updates

### Person C (Fabiola) - Admin System
- Admin console thread
- Entity creation/deletion commands
- Factory pattern
- EntityManager
- **Deliverable**: Admin interface

### Person D (Nicolás) - Client Graphics
- Raylib integration
- Game rendering
- Input handling
- Protocol parsing
- **Deliverable**: Graphical client

---

## 📝 User Stories for Sprint 1

### Epic 1: Game State Management
```
US1.1: As a server, I need to maintain game state with all entity positions
US1.2: As a server, I need to detect collisions between entities
US1.3: As a server, I need to track player lives and scores
```

### Epic 2: Player Control
```
US2.1: As a player, I want to move using arrow keys
US2.2: As a player, I want to jump to grab vines
US2.3: As a player, I want to see my lives and score
```

### Epic 3: Enemy Behavior
```
US3.1: As a server, I need red crocodiles to patrol vertically on vines
US3.2: As a server, I need blue crocodiles to fall down vines
US3.3: As a server, I need to increase enemy speed after level completion
```

### Epic 4: Admin Controls
```
US4.1: As an admin, I want to create crocodiles via console
US4.2: As an admin, I want to create fruits with custom points
US4.3: As an admin, I want to delete fruits
```

### Epic 5: Spectator Experience
```
US5.1: As a spectator, I want to see real-time game state
US5.2: As a spectator, I want read-only view (no input)
US5.3: As a spectator, I want notification when player disconnects
```

---

## 🚀 Implementation Priorities

### Week 1 (Parallel Development)
1. **Person A**: Create entity classes & GameState
2. **Person D**: Set up Raylib, create rendering system
3. **Person B**: Design command pattern structure
4. **Person C**: Build admin console interface

### Week 2 (Integration)
1. Connect GameState to GameLoop
2. Link commands to state updates
3. Wire admin console to entity creation
4. Connect client renderer to network updates

### Week 3 (Polish & Testing)
1. Full system testing
2. Performance optimization
3. Documentation
4. Bug fixes

---

## ⚠️ Risk Assessment

### Technical Risks
1. **Synchronization Issues**: Multiple threads accessing GameState
   - **Mitigation**: Use synchronized methods or locks

2. **Network Latency**: State updates might lag
   - **Mitigation**: Send deltas instead of full state, interpolation

3. **Collision Detection Performance**: O(n²) with many entities
   - **Mitigation**: Spatial partitioning, limit entity count

### Process Risks
1. **Integration Delays**: Interfaces not matching
   - **Mitigation**: Define contracts early, use mocks

2. **Unequal Work Distribution**: Some finish early, others blocked
   - **Mitigation**: Clear task boundaries, help with integration

---

## 📊 Success Metrics

### Functional Requirements
- [ ] 2 players can connect and play simultaneously
- [ ] Spectators can watch without affecting game
- [ ] Admin can create/delete entities during gameplay
- [ ] Collision detection works correctly
- [ ] Score and lives tracked accurately
- [ ] Level progression with speed increase

### Technical Requirements
- [ ] Java uses 2+ design patterns (excluding Singleton)
- [ ] C uses structs and separate constants file
- [ ] No primitive types in Java (wrapper classes only)
- [ ] Clean separation of concerns
- [ ] Thread-safe operations

### Quality Metrics
- [ ] 60 FPS game loop
- [ ] <100ms network latency
- [ ] No memory leaks
- [ ] Graceful disconnect handling
- [ ] Clear error messages

---

## 🎯 Meeting Agenda Topics

1. **Protocol Finalization** (30 min)
   - Review proposed message formats
   - Agree on state JSON structure
   - Define command syntax

2. **Interface Contracts** (45 min)
   - GameState methods (Person A)
   - Command interface (Person B)
   - EntityManager API (Person C)
   - Network protocol (Person D)

3. **Development Timeline** (15 min)
   - Week 1 deliverables
   - Integration checkpoints
   - Testing schedule

4. **Technical Decisions** (20 min)
   - Collision detection algorithm
   - State broadcast frequency
   - Coordinate system (pixels vs grid)
   - Vine representation

5. **Risk Mitigation** (10 min)
   - Backup plans
   - Help priorities
   - Communication channels

---

## 💡 Key Decisions Needed

1. **Coordinate System**
   - Pixel-based (float x, y)?
   - Grid-based (int row, col)?
   - Hybrid (vine ID + height)?

2. **State Updates**
   - Full state every frame?
   - Delta updates only?
   - Hybrid (full + deltas)?

3. **Collision Boundaries**
   - Rectangle hitboxes?
   - Circle/radius?
   - Pixel-perfect?

4. **Admin Interface**
   - Console commands only?
   - GUI window?
   - Web interface?

5. **Graphics Library**
   - Raylib (proposed)?
   - SDL2?
   - ncurses (text-based)?

---

## 📚 Resources & References

### Documentation
- [Socket Programming C/Java](https://old.chuidiang.org/java/sockets/cpp_java/cpp_java.php)
- [Raylib Documentation](https://www.raylib.com/)
- [Java Design Patterns](https://refactoring.guru/design-patterns/java)

### Testing Tools
- Valgrind (C memory leaks)
- JUnit (Java unit tests)
- Wireshark (network debugging)

### Similar Projects
- [Original Donkey Kong Jr](https://www.retrogames.cz/play_002-NES.php)
- Multiplayer game architectures
- Real-time synchronization techniques

---

## ✅ Pre-Meeting Checklist

- [ ] Each member reviews their Tasks.md section
- [ ] Prepare questions about unclear requirements
- [ ] Think about interface boundaries
- [ ] Consider potential blockers
- [ ] Review current codebase
- [ ] Test network connection works
- [ ] Prepare development environment

---

## 📝 Notes Section

*Use this space during the meeting to capture decisions and action items*

### Decisions Made:

## 🖥️ SERVIDOR (Java)

### Conexiones
- Aceptar conexiones TCP en puerto 12345
- Validar tipo de cliente (jugador/espectador)
- Máximo 2 jugadores simultáneos
- Máximo 2 espectadores por jugador
- Asociar espectadores con jugadores específicos

### Admin UI (Java Swing)
- Panel con dropdown de tipos de enemigo (Rojo/Azul)
- Dropdown de número de liana (1-5)
- Campo de texto para posición Y de frutas
- Botón "Crear Enemigo"
- Botón "Crear Fruta"
- Botón "Eliminar Fruta"
- Lista visual de frutas creadas
- Lista de jugadores conectados con sus IPs

### Lógica de Enemigos
- **Cocodrilos Rojos**: Spawnan en liana específica, suben y bajan entre límites de la liana, nunca caen
- **Cocodrilos Azules**: Spawnan desde los pies de Mario (parte superior), caen verticalmente hasta atravesar el piso y llegar al agua
- Actualizar posiciones de enemigos a 60 FPS
- Broadcast de posiciones cada 100ms (10 Hz)
- Eliminar enemigos azules cuando caen al agua
- Aumentar velocidad de enemigos después de cada nivel completado

### Comunicación
- Broadcast inicial de estructura de nivel (lianas, plataformas, posición de jaula)
- Broadcast cuando admin crea/elimina entidades
- Retransmitir estado de jugadores a sus espectadores
- Recibir eventos de clientes (muerte, frutas, nivel completado) para logging

### Patrones de Diseño (mínimo 2)
- **Observer**: Espectadores observan jugadores
- **Factory**: EntityFactory para crear enemigos/frutas
- **Thread-per-Connection**: ClientHandler por cliente

### Estructura OO
- Paquetes: `network`, `game`, `ui`, `utils`
- Clases: `GameServer`, `ClientHandler`, `Enemy`, `Fruit`, `EntityFactory`, `ServerUI`
- Sin tipos primitivos (usar wrappers: Integer, Float, Boolean)

---

## 💻 CLIENTE (C + Raylib)

### Lobby
- Mostrar menú: 1) Jugador, 2) Espectador, 3) Salir
- Enviar selección al servidor
- Si espectador: mostrar lista de jugadores disponibles
- Esperar confirmación de aceptación/rechazo

### Input (solo para jugadores)
- Flechas izquierda/derecha: Movimiento horizontal
- Flecha arriba: Trepar liana (si está agarrado)
- Flecha abajo: Bajar liana / soltar
- Espacio: Saltar / agarrar liana cercana

### Física del Jugador (local)
- Gravedad constante cuando no está en piso ni trepando
- Velocidad horizontal constante al moverse
- Velocidad de trepar constante
- Snap a plataforma cuando aterriza
- Detener movimiento horizontal al chocar con pared

### Sistema de Lianas
- Recibir estructura de lianas del servidor al inicio
- Guardar posiciones de lianas visibles e invisibles
- Detectar lianas cercanas para agarrar (radio de alcance)
- Permitir agarrar lianas invisibles solo si lianas laterales están suficientemente bajas
- Soltar liana al presionar abajo hasta no tocar ninguna
- Soltar liana al moverse lateralmente fuera de rango

### Detección de Colisiones (local)
- **Piso/Plataformas**: Si Y del jugador >= Y del piso → snap arriba del piso
- **Enemigos**: Si distancia < radio de colisión → pierde vida, respawn
- **Frutas**: Si overlap → agrega puntos, elimina fruta localmente
- **Agua**: Si Y > nivel del agua → muerte, respawn
- **Jaula de Donkey Kong**: Si overlap → victoria, +1 vida, reset nivel

### Sistema de Vidas y Puntuación (local)
- Empezar con 3 vidas
- Perder vida al: chocar enemigo, caer al agua
- Ganar vida al: completar nivel
- Sumar puntos al: recoger frutas
- Mostrar UI: vidas restantes, puntuación actual

### Renderizado
- Sprites según estado: idle, caminando, saltando, trepando, muerto
- Sprites según dirección: izquierda, derecha
- Animación de caminar (4 frames)
- Animación de trepar (2 frames)
- Renderizar lianas visibles (las invisibles no se dibujan)
- Renderizar plataformas
- Renderizar enemigos con interpolación
- Renderizar frutas
- Renderizar jaula de Donkey Kong
- Renderizar UI (vidas, score)

### Comunicación
- Recibir estructura de nivel al inicio
- Recibir posiciones de enemigos (10 Hz) y interpolar para 60 FPS
- Recibir notificaciones de nuevas entidades (enemigos/frutas)
- Enviar estado propio cada 50-100ms: posición, estado, vidas, score
- Enviar eventos importantes: muerte, fruta recogida, nivel completado

### Espectador (modo read-only)
- NO procesar input de teclado
- Recibir estado del jugador observado
- Renderizar estado recibido
- Mostrar notificación si jugador se desconecta

### Estructura Imperativa
- Usar structs para: Player, Enemy, Fruit, Vine, Platform, GameState
- Archivo `constants.h` separado con todas las constantes
- Sin clases ni POO
- Funciones puras que operan sobre structs

---

## 🌐 PROTOCOLO DE RED

### Cliente → Servidor
```
"JOIN:PLAYER"
"JOIN:SPECTATOR:<playerId>"
"STATE:<x>,<y>:<estado>:<facing>:<vidas>:<score>"
"EVENT:DIED:ENEMY"
"EVENT:DIED:FALL"
"EVENT:FRUIT:<puntos>"
"EVENT:LEVEL_COMPLETE"
```

### Servidor → Todos los Clientes
```
"LEVEL_DATA:<json>"
"ENEMY_SPAWNED:<tipo>:<lianaId>:<y>"
"FRUIT_SPAWNED:<lianaId>:<altura>:<puntos>"
"FRUIT_REMOVED:<lianaId>:<altura>"
"ENEMIES:<id>,<x>,<y>|<id>,<x>,<y>|..."
```

### Servidor → Espectadores Específicos
```
"PLAYER_STATE:<x>,<y>:<estado>:<facing>:<vidas>:<score>"
"PLAYER_EVENT:DIED"
"PLAYER_EVENT:LEVEL_COMPLETE"
"PLAYER_DISCONNECTED"
```

---

## 🎮 MECÁNICAS DE JUEGO

### Enemigos
- **Rojos**: Patrullan verticalmente en una liana, cambian dirección al llegar a límites
- **Azules**: Spawnan arriba (pies de Mario), caen hasta agua, no caminan, no rebotan
- Velocidad base: configurable
- Multiplicador de velocidad: incrementa +20% cada nivel completado

### Frutas
- Posicionadas en lianas específicas con altura relativa (0.0 = arriba, 1.0 = abajo)
- Puntos configurables por fruta
- Se eliminan al ser recogidas (no reaparecen)

### Win Condition
- Llegar específicamente a la jaula de Donkey Kong (no solo la cima)
- Otorga +1 vida
- Reinicia nivel con enemigos más rápidos
- Frutas reaparecen en posiciones originales

### Pérdida
- Perder todas las vidas = Game Over
- Game Over permite reiniciar o volver al lobby

---

## 📁 ESTRUCTURA DE ARCHIVOS

### Servidor
```
server/
├── src/
│   ├── Main.java
│   ├── network/
│   │   ├── GameServer.java
│   │   ├── ClientHandler.java
│   │   └── NetworkPlayer.java
│   ├── game/
│   │   ├── entities/
│   │   │   ├── Enemy.java
│   │   │   ├── RedEnemy.java
│   │   │   ├── BlueEnemy.java
│   │   │   └── Fruit.java
│   │   └── managers/
│   │       └── EntityFactory.java
│   ├── ui/
│   │   └── ServerUI.java
│   └── utils/
│       └── Config.java
└── bin/
```

### Cliente
```
client/
├── src/
│   ├── main.c
│   ├── network/
│   │   ├── connection.c/h
│   │   ├── lobby_handler.c/h
│   │   └── session_handler.c/h
│   ├── game/
│   │   ├── entities.c/h
│   │   ├── physics.c/h
│   │   ├── collision.c/h
│   │   └── state_parser.c/h
│   ├── rendering/
│   │   ├── renderer.c/h
│   │   └── sprites.c/h
│   ├── input/
│   │   └── input_handler.c/h
│   └── utils/
│       ├── constants.h
│       └── types.h
├── assets/
│   └── sprites/
└── Makefile
```

---

## ✅ CUMPLIMIENTO DE RÚBRICA

### Servidor (Puntos 1-9)
- ✅ Crear enemigos rojos (admin UI)
- ✅ Crear enemigos azules (admin UI)
- ✅ Crear/eliminar frutas (admin UI)
- ✅ Velocidad de enemigos (IA + multiplicador)
- ✅ OO (paquetes, clases)
- ✅ Patrones (mínimo 2)
- ✅ Ejecutable

### Cliente (Puntos 10-18)
- ✅ Control de Donkey (input + física)
- ✅ 2 jugadores activos
- ✅ 2 espectadores por jugador
- ✅ Interpretación de estructura servidor
- ✅ Informa muerte/frutas (eventos)
- ✅ Archivo constantes
- ✅ Imperativo (structs)
- ✅ Ejecutable

### Comunicación (Punto 22)
- ✅ Sockets TCP bidireccional



### Action Items:
- 

### Questions to Research:
- 

### Next Meeting:
- Date:
- Topics:

---