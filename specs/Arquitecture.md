# DonCEy Kong Jr - Project Structure

## 📂 Current Structure

```
DonCEy-Kong-JR/
├── README.md
├── .gitignore
│
├── server/                          # Java Server
│   ├── src/
│   │   ├── Main.java               # Server entry point
│   │   │
│   │   ├── network/                # Network layer ✅
│   │   │   ├── GameServer.java    # Main server logic
│   │   │   ├── ClientHandler.java # Thread-per-connection
│   │   │   └── NetworkPlayer.java # Player DTO
│   │   │
│   │   └── utils/                  # Utilities ✅
│   │       └── Config.java        # Server configuration
│   │
│   └── bin/                        # Compiled .class files
│
├── client/                         # C Client
│   ├── src/
│   │   ├── main.c                 # Client entry point ✅
│   │   │
│   │   ├── network/               # Network layer ✅
│   │   │   ├── connection.c/h     # Socket connection
│   │   │   ├── lobby_handler.c/h  # Lobby phase logic
│   │   │   └── session_handler.c/h # Game session logic
│   │   │
│   │   └── utils/                 # Utilities ✅
│   │       └── constants.h        # Protocol & config constants
│   │
│   ├── build/                     # Compiled executables
│   │   └── client
│   │
│   └── Makefile                   # Build system ✅
│
├── test/                          # Test files ✅
│   ├── ServerTest.java           # Java server test
│   └── ClientTest.c              # C client test
│
└── specs/                         # Documentation ✅
    ├── Arquitecture.md           # Architecture proposal
    ├── Patterns.md               # Design patterns used
    └── Tasks.md                  # Team task distribution
```

---

## 🚀 Future Structure (Game Implementation)

```
DonCEy-Kong-JR/
├── server/
│   ├── src/
│   │   ├── Main.java               ✅ Current
│   │   │
│   │   ├── network/                ✅ Current
│   │   │   ├── GameServer.java
│   │   │   ├── ClientHandler.java  🔧 Needs modification for commands
│   │   │   └── NetworkPlayer.java
│   │   │
│   │   ├── game/                   🆕 NEW - Game Logic Layer
│   │   │   │
│   │   │   ├── entities/           🆕 Person A (Tamara)
│   │   │   │   ├── Entity.java           # Abstract base class
│   │   │   │   ├── Position.java         # Position/velocity data
│   │   │   │   ├── GamePlayer.java       # Player entity (not NetworkPlayer)
│   │   │   │   ├── Crocodile.java        # Abstract enemy
│   │   │   │   │   ├── RedCrocodile.java # Vertical patrol
│   │   │   │   │   └── BlueCrocodile.java# Falling enemy
│   │   │   │   └── Fruit.java            # Collectible item
│   │   │   │
│   │   │   ├── state/              🆕 Person A (Tamara)
│   │   │   │   └── GameState.java        # Central game state manager
│   │   │   │
│   │   │   ├── commands/           🆕 Person B (David)
│   │   │   │   ├── Command.java          # Command interface
│   │   │   │   ├── MoveCommand.java      # UP/DOWN/LEFT/RIGHT
│   │   │   │   ├── JumpCommand.java      # Space bar action
│   │   │   │   └── ClimbCommand.java     # Grab vine action
│   │   │   │
│   │   │   ├── managers/           🆕 Person A & C
│   │   │   │   ├── CollisionManager.java # Person A - Collision detection
│   │   │   │   └── EntityManager.java    # Person C - Entity CRUD
│   │   │   │
│   │   │   └── GameLoop.java       🆕 Person B (David)
│   │   │                                 # Main game loop (60 FPS)
│   │   │
│   │   ├── admin/                  🆕 Person C (Fabiola)
│   │   │   ├── AdminConsole.java         # Admin command interface
│   │   │   └── EntityFactory.java        # Factory pattern for entities
│   │   │
│   │   └── utils/                  ✅ Current
│   │       └── Config.java
│   │
│   └── bin/
│
├── client/
│   ├── src/
│   │   ├── main.c                  🔧 Needs modification for game loop
│   │   │
│   │   ├── network/                ✅ Current
│   │   │   ├── connection.c/h
│   │   │   ├── lobby_handler.c/h
│   │   │   └── session_handler.c/h 🔧 Will call game rendering
│   │   │
│   │   ├── game/                   🆕 Person D (Nicolás) - Game State
│   │   │   ├── entities.h/c              # Player, Crocodile, Fruit structs
│   │   │   ├── game_state.h/c            # Local game state mirror
│   │   │   └── state_parser.h/c          # Parse server protocol
│   │   │
│   │   ├── rendering/              🆕 Person D (Nicolás) - Graphics
│   │   │   ├── renderer.h/c              # Main render loop
│   │   │   ├── sprite_manager.h/c        # Load/manage sprites
│   │   │   └── animation.h/c             # Animation system
│   │   │
│   │   ├── input/                  🆕 Person D (Nicolás) - Input
│   │   │   ├── input_handler.h/c         # Keyboard input detection
│   │   │   └── command_builder.h/c       # Convert input → protocol
│   │   │
│   │   └── utils/                  ✅ Current
│   │       ├── constants.h         🔧 Will add game constants
│   │       └── types.h             🆕 Common type definitions
│   │
│   ├── assets/                     🆕 Person D (Nicolás) - Resources
│   │   ├── sprites/
│   │   │   ├── player/                   # Player sprite sheets
│   │   │   ├── enemies/                  # Crocodile sprites
│   │   │   ├── environment/              # Vines, platforms
│   │   │   └── items/                    # Fruit sprites
│   │   │
│   │   └── fonts/                        # UI fonts
│   │
│   ├── build/
│   └── Makefile                    🔧 Will add Raylib linking
│
├── test/                           ✅ Current
│   ├── ServerTest.java
│   └── ClientTest.c
│
└── specs/                          ✅ Current
    ├── Arquitecture.md
    ├── Patterns.md
    └── Tasks.md
```
---