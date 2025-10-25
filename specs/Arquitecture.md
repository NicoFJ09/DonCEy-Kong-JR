donceykong-jr/
├── README.md
├── .gitignore
│
├── server/                          # Proyecto Java
│   ├── src/
│   │   └── com/
│   │       └── donceykong/
│   │           ├── Main.java        # Entry point del servidor
│   │           │
│   │           ├── core/            # Núcleo del servidor
│   │           │   ├── GameServer.java
│   │           │   └── GameLoop.java (futuro)
│   │           │
│   │           ├── network/         # Todo lo relacionado con red
│   │           │   ├── GameObserver.java
│   │           │   ├── ClientHandler.java
│   │           │   └── MessageProtocol.java (futuro)
│   │           │
│   │           ├── commands/        # Command Pattern
│   │           │   ├── Command.java
│   │           │   ├── MoveCommand.java (futuro)
│   │           │   └── JumpCommand.java (futuro)
│   │           │
│   │           ├── entities/        # Entidades del juego (futuro)
│   │           │   ├── Entity.java
│   │           │   ├── Player.java
│   │           │   ├── Crocodile.java
│   │           │   └── Fruit.java
│   │           │
│   │           ├── managers/        # Gestores (futuro)
│   │           │   ├── EntityManager.java
│   │           │   └── CollisionManager.java
│   │           │
│   │           └── utils/           # Utilidades
│   │               ├── Config.java
│   │               └── Logger.java
│   │
│   ├── bin/                         # Compilados (.class)
│   └── lib/                         # Librerías externas (si usas)
│
├── client/                          # Proyecto C
│   ├── src/
│   │   ├── main.c                   # Entry point jugador
│   │   ├── spectator_main.c         # Entry point espectador
│   │   │
│   │   ├── network/                 # Comunicación con servidor
│   │   │   ├── connection.c
│   │   │   ├── connection.h
│   │   │   ├── message_handler.c
│   │   │   └── message_handler.h
│   │   │
│   │   ├── game/                    # Lógica local del juego
│   │   │   ├── game_state.c
│   │   │   ├── game_state.h
│   │   │   ├── entities.c
│   │   │   └── entities.h
│   │   │
│   │   ├── rendering/               # Todo lo gráfico
│   │   │   ├── renderer.c
│   │   │   ├── renderer.h
│   │   │   ├── sprite_manager.c
│   │   │   ├── sprite_manager.h
│   │   │   ├── animation.c
│   │   │   └── animation.h
│   │   │
│   │   ├── input/                   # Manejo de teclado
│   │   │   ├── keyboard.c
│   │   │   ├── keyboard.h
│   │   │   ├── input_handler.c
│   │   │   └── input_handler.h
│   │   │
│   │   └── utils/                   # Utilidades
│   │       ├── constants.h
│   │       ├── types.h
│   │       └── logger.c/h
│   │
│   ├── include/                     # Headers de librerías externas
│   │   └── raylib.h (si no está en sistema)
│   │
│   ├── assets/                      # Recursos gráficos
│   │   ├── sprites/
│   │   │   ├── player/
│   │   │   ├── enemies/
│   │   │   ├── environment/
│   │   │   └── items/
│   │   ├── fonts/
│   │   └── sounds/ (opcional)
│   │
│   ├── build/                       # Ejecutables compilados
│   │
│   └── Makefile                     # Build system
│
├── specs/                            # Documentación
│   ├── architecture.md
│   ├── protocol.md
│   ├── manual_usuario.md
│   └── bitacora.md