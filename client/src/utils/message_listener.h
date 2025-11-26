#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include "../network/connection.h"
#include <pthread.h>
#include <stdbool.h>

// Forward declaration (avoid circular dependency)
struct Level;

// Message listener runs in separate thread to avoid blocking main thread
// Handles server responses for game events (score updates, lives, etc)
// Also queues admin commands for processing in main thread

// Callback function type for handling server messages
typedef void (*MessageCallback)(const char* message, void* user_data);

// Start listening for messages with optional callback
pthread_t message_listener_start(Connection* conn);

// Set callback for handling messages
void message_listener_set_callback(MessageCallback callback, void* user_data);

void message_listener_stop(pthread_t thread_id);

// ============================================================
// ADMIN COMMAND PROCESSING
// Call once per frame from main thread (player_screen.c)
// ============================================================

/**
 * Process pending admin commands from server
 * Thread-safe: dequeues commands from listener thread
 * @param level Current level (for spawning entities)
 * @param conn Connection (for sending confirmations - Phase 2/3)
 */
void message_listener_process_admin_commands(struct Level* level, Connection* conn);

#endif
