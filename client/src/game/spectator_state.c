#include "spectator_state.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

StateBuffer* state_buffer_create(void) {
    StateBuffer* buffer = malloc(sizeof(StateBuffer));
    if (!buffer) return NULL;
    
    buffer->write_index = 0;
    buffer->read_index = 0;
    buffer->count = 0;
    pthread_mutex_init(&buffer->mutex, NULL);
    
    return buffer;
}

void state_buffer_push(StateBuffer* buffer, GameStateSnapshot* snapshot) {
    if (!buffer || !snapshot) return;
    
    pthread_mutex_lock(&buffer->mutex);
    
    buffer->snapshots[buffer->write_index] = *snapshot;
    buffer->write_index = (buffer->write_index + 1) % STATE_BUFFER_SIZE;
    
    if (buffer->count < STATE_BUFFER_SIZE) {
        buffer->count++;
    } else {
        // Buffer lleno, avanzar read_index
        buffer->read_index = (buffer->read_index + 1) % STATE_BUFFER_SIZE;
    }
    
    pthread_mutex_unlock(&buffer->mutex);
}

bool state_buffer_pop(StateBuffer* buffer, GameStateSnapshot* out_snapshot) {
    if (!buffer || !out_snapshot) return false;
    
    pthread_mutex_lock(&buffer->mutex);
    
    if (buffer->count == 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return false;
    }
    
    *out_snapshot = buffer->snapshots[buffer->read_index];
    buffer->read_index = (buffer->read_index + 1) % STATE_BUFFER_SIZE;
    buffer->count--;
    
    pthread_mutex_unlock(&buffer->mutex);
    return true;
}

void state_buffer_free(StateBuffer* buffer) {
    if (!buffer) return;
    pthread_mutex_destroy(&buffer->mutex);
    free(buffer);
}