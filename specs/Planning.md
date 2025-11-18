# DonCEy Kong Jr - Project Analysis & Team Meeting Preparation

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

### Epic 2: Player Control
```
US2.1: As a player, I want to move using arrow keys
US2.2: As a player, I want to jump to grab vines
US2.3: As a player, I want to see my lives and score
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

## Requirements:

### 🖥️ SERVIDOR (Java)

### Conexiones
- Aceptar conexiones TCP en puerto
- Validar tipo de cliente (jugador/espectador)
- Máximo 2 jugadores simultáneos
- Máximo 2 espectadores por jugador
- Asociar espectadores con jugadores específicos

### Admin UI (Java Swing)
- Panel con dropdown de tipos de enemigo (Rojo/Azul)
- Dropdown de número de liana (1-5)
- Campo de texto para tipo de fruta
- Campo de texto para posición Y de frutas
- Botón "Crear Enemigo"
- Botón "Crear Fruta"
- Botón "Eliminar Fruta"
- Lista visual de frutas creadas (permite selección)
- Lista de jugadores conectados con sus IPs (actualización automática)

### Variables Persistentes (por jugador)
- **Vidas**: Inicia en 3, decrementa al recibir `EVENT:DIED`, envía `LIVES:<n>`
- **Score**: Inicia en 0, incrementa al recibir `EVENT:FRUIT:<puntos>`, envía `SCORE:<n>`
- **Velocidad**: Inicia en 1.0, incrementa 20% al recibir `EVENT:LEVEL_COMPLETE`, envía `SPEED:<n>`
- **Game Over**: Cuando vidas = 0, envía `GAME_OVER`

### Broadcast de Entidades (solo creación/eliminación)
- Admin crea enemigo → broadcast a TODOS: `ENEMY_SPAWNED:<tipo>:<lianaId>`
- Admin crea fruta → broadcast a TODOS: `FRUIT_SPAWNED:<lianaId>:<altura>:<puntos>`
- Admin elimina fruta → broadcast a TODOS: `FRUIT_REMOVED:<lianaId>:<altura>`
- Servidor NO actualiza ni envía posiciones después de crear

### Comunicación con Jugadores
- Recibir eventos: `EVENT:DIED`, `EVENT:FRUIT:<puntos>`, `EVENT:LEVEL_COMPLETE`
- Enviar confirmaciones: `LIVES:<n>`, `SCORE:<n>`, `SPEED:<n>`, `GAME_OVER`
- Recibir estado completo (20 Hz): `STATE:<json>`
- NO validar física, colisiones ni posiciones

### Comunicación con Espectadores (relay)
- Recibir estado del jugador observado
- Retransmitir sin modificar: `PLAYER_STATE:<json>` (json con todas las entidades)
- Retransmitir eventos: `PLAYER_EVENT:DIED`, `PLAYER_EVENT:LEVEL_COMPLETE`
- Enviar `PLAYER_DISCONNECTED` si jugador se desconecta

### Patrones de Diseño (mínimo 2)
- **Observer**: Espectadores observan jugadores, servidor notifica cambios
- **Factory**: EntityFactory crea instancias según tipo
- **Thread-per-Connection**: ClientHandler extiende Thread

### Estructura OO
- Paquetes: `network`, `game/entities`, `game/managers`, `ui`, `utils`
- Herencia: `Enemy` (abstract) → `RedEnemy`, `BlueEnemy`
- Sin tipos primitivos: `Integer`, `Float`, `Boolean`, `String`

---

### 💻 CLIENTE (C + Raylib)

### Lobby
- Mostrar menú: 1) Jugador, 2) Espectador, 3) Salir
- Enviar `JOIN:PLAYER` o `JOIN:SPECTATOR:<playerId>`
- Si espectador: mostrar lista de jugadores disponibles
- Esperar `ACCEPTED` o `REJECTED`
- Transicionar a juego si aceptado

### Input (solo jugadores, NO espectadores)
- Flechas izquierda/derecha: Movimiento horizontal
- Flecha arriba: Trepar liana (si agarrado)
- Flecha abajo: Bajar liana / soltar
- Espacio: Saltar / agarrar liana cercana

### Física del Jugador (local, 60 FPS)
- Gravedad constante cuando no está en piso ni trepando
- Velocidad horizontal constante al moverse
- Velocidad de trepar constante
- Snap a plataforma cuando aterriza
- Detener movimiento horizontal al chocar con pared

### Sistema de Lianas
- Recibir estructura inicial: `LEVEL_DATA:<json>`
- Guardar posiciones de lianas visibles e invisibles
- Detectar lianas cercanas para agarrar (radio de alcance)
- Permitir agarrar lianas invisibles solo si lianas laterales están suficientemente bajas
- Soltar liana al presionar abajo hasta no tocar ninguna
- Soltar liana al moverse lateralmente fuera de rango

### Lógica de Enemigos (local, 60 FPS)
- Recibir creación: `ENEMY_SPAWNED:<tipo>:<lianaId>`
- Crear enemigo en lista local y actualizarlo desde ese momento
- **Rojos**: Patrullan verticalmente, invierten dirección en límites
- **Azules**: Caen verticalmente, eliminarse al tocar agua
- Aplicar multiplicador `game_speed` recibido del servidor
- Cliente tiene TODA la lógica de movimiento

### Detección de Colisiones (local)
- **Piso/Plataformas**: Snap arriba del piso cuando aterriza
- **Paredes**: Detener movimiento horizontal
- **Enemigos**: Muerte si distancia < radio
- **Frutas**: Recoger si overlap
- **Agua**: Muerte si Y > nivel del agua
- **Jaula**: Victoria si overlap con rectángulo

### Eventos al Servidor
- Muerte: `EVENT:DIED:ENEMY` o `EVENT:DIED:FALL`
- Fruta: `EVENT:FRUIT:<puntos>`
- Victoria: `EVENT:LEVEL_COMPLETE`
- Servidor responde con confirmación

### Sistema de Vidas y Puntuación
- Variables locales: `lives`, `score`, `game_speed`
- Actualizar localmente al detectar evento
- Enviar evento al servidor
- Recibir confirmación y sincronizar si difiere

### Estado para Espectadores (envío periódico)
- Cada 50-100ms enviar estado completo al servidor:
```json
STATE:{
  player: {x, y, state, facing},
  enemies: [{id, x, y}, ...],
  fruits: [{id, x, y}, ...],
  lives,
  score
}
```
- Servidor retransmite a espectadores sin procesar

### Renderizado (60 FPS)
- Sprites según estado: idle, walk, jump, climb, dead
- Sprites según dirección: left, right
- Animaciones: caminar (4 frames), trepar (2 frames)
- Renderizar lianas visibles (invisibles NO se dibujan)
- Renderizar plataformas, enemigos, frutas, jaula
- UI: vidas, score, nivel

### Modo Espectador
- NO procesar input de teclado (solo ESC para salir)
- Recibir estado completo: `PLAYER_STATE:<json>`
- Interpolar entre actualizaciones (20 Hz → 60 FPS)
- Mostrar overlay: "Observando a Jugador #<id>"
- Detectar `PLAYER_DISCONNECTED`

### Estructura Imperativa
- Structs: `Player`, `Enemy`, `Fruit`, `Vine`, `Platform`, `GameState`
- Archivo `constants.h` con todas las constantes
- Sin clases: Solo funciones sobre structs
- Paso por puntero: `void update_player(Player* p, float dt)`

---

## 🌐 PROTOCOLO DE RED

### Cliente → Servidor (Jugador)
```
JOIN:PLAYER
EVENT:DIED:ENEMY
EVENT:DIED:FALL
EVENT:FRUIT:<puntos>
EVENT:LEVEL_COMPLETE
STATE:<json>
```

### Cliente → Servidor (Espectador)
```
JOIN:SPECTATOR:<playerId>
```

### Servidor → Todos los Clientes (broadcast)
```
ENEMY_SPAWNED:<tipo>:<lianaId>
FRUIT_SPAWNED:<lianaId>:<altura>:<puntos>
FRUIT_REMOVED:<lianaId>:<altura>
LEVEL_DATA:<json>
```

### Servidor → Jugador Específico (confirmaciones)
```
ACCEPTED:PLAYER
REJECTED:<razón>
LIVES:<n>
SCORE:<n>
SPEED:<n>
GAME_OVER
```

### Servidor → Espectador Específico (relay)
```
ACCEPTED:SPECTATOR
REJECTED:<razón>
PLAYER_STATE:<json>
PLAYER_EVENT:DIED
PLAYER_EVENT:LEVEL_COMPLETE
PLAYER_DISCONNECTED
```

---

## 🎮 MECÁNICAS DE JUEGO

### Enemigos
- **Rojos**: Patrullan verticalmente en liana, invierten dirección en límites, nunca caen
- **Azules**: Spawnan arriba, caen hasta agua, atraviesan plataformas
- Velocidad: `base_speed * game_speed` (game_speed del servidor)
- Lógica completamente en cliente después de spawn

### Frutas
- Posición: Liana + altura relativa (0.0-1.0)
- Puntos configurables en admin
- Cliente elimina localmente al recoger
- Reaparecen al completar nivel

### Win Condition
- Colisión con jaula de Donkey Kong
- Cliente envía `EVENT:LEVEL_COMPLETE`
- Servidor responde: `LIVES:<n+1>`, `SCORE:<score+bonus>`, `SPEED:<speed*1.2>`
- Cliente resetea nivel localmente

### Pérdida
- Vidas = 0 → Servidor envía `GAME_OVER`
- Pantalla de Game Over
- Opciones: Reiniciar o volver al lobby

---

## ✅ CUMPLIMIENTO DE RÚBRICA

### Servidor (Puntos 1-9)
- ✅ 1-2. Crear enemigos rojos/azules (admin UI → broadcast)
- ✅ 3. Crear/eliminar frutas (admin UI → broadcast)
- ✅ 4. Administración de vidas (contador persistente)
- ✅ 5. Administración de puntuación (contador persistente)
- ✅ 6. Velocidad de desplazamiento (multiplicador, incremento por nivel)
- ✅ 7. OO (herencia, paquetes)
- ✅ 8. Patrones (Observer, Factory, Thread-per-Connection)
- ✅ 9. Ejecutable

### Cliente (Puntos 10-18)
- ✅ 10. Control de Donkey (input + física local)
- ✅ 11. 2 jugadores activos (2 clientes jugadores)
- ✅ 12. 2 espectadores por jugador (4 clientes espectadores)
- ✅ 13. Interpretación estructura servidor (parse mensajes)
- ✅ 14. Informa muerte (envía EVENT:DIED)
- ✅ 15. Informa frutas (envía EVENT:FRUIT)
- ✅ 16. Archivo constantes (constants.h)
- ✅ 17. Imperativo (structs, funciones)
- ✅ 18. Ejecutable

### Comunicación (Punto 22)
- ✅ Sockets TCP (C ↔ Java)