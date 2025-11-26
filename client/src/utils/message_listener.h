#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include "../network/connection.h"
#include <pthread.h>
#include <stdbool.h>

// Message listener runs in separate thread to avoid blocking main thread
// Handles server responses for game events (score updates, lives, etc)

// Callback function type for handling server messages
typedef void (*MessageCallback)(const char* message, void* user_data);

// Start listening for messages with optional callback
pthread_t message_listener_start(Connection* conn);

// Set callback for handling messages
void message_listener_set_callback(MessageCallback callback, void* user_data);

void message_listener_stop(pthread_t thread_id);

#endif
