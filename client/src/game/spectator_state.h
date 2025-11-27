#ifndef SPECTATOR_STATE_H
#define SPECTATOR_STATE_H

#include <stdbool.h>
#include <pthread.h>
#include "player.h"
#include "enemy.h"

typedef enum {
    ENTITY_PLAYER,
    ENTITY_ENEMY_RED,
    ENTITY_ENEMY_BLUE,
    ENTITY_FRUIT
} EntityType;

typedef struct {
    int id;
    EntityType type;
    float x;
    float y;
    PlayerState player_state;  // For players
    EnemyState enemy_state;    // For enemies
    bool active;
    int direction; // -1 izquierda, 1 derecha
} EntitySnapshot;

typedef struct {
    int score;
    int lives;
    int level;
    EntitySnapshot entities[100]; // Define MAX_ENTITIES como 100 o similar
    int entity_count;
    double timestamp;
} GameStateSnapshot;


#define STATE_BUFFER_SIZE 30  // ~0.5 segundos a 60fps
#define MAX_ENTITIES 100

typedef struct {
    GameStateSnapshot snapshots[STATE_BUFFER_SIZE];
    int write_index;
    int read_index;
    int count;
    pthread_mutex_t mutex;
} StateBuffer;

// Funciones del buffer
StateBuffer* state_buffer_create(void);
void state_buffer_push(StateBuffer* buffer, GameStateSnapshot* snapshot);
bool state_buffer_pop(StateBuffer* buffer, GameStateSnapshot* out_snapshot);
void state_buffer_free(StateBuffer* buffer);

#endif
