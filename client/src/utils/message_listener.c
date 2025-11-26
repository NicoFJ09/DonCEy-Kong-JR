#include "message_listener.h"
#include "../utils/constants.h"
#include "../game/level.h"
#include "../game/enemy.h"
#include "../game/fruit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// External reference to current level (defined in player_screen.c)
extern Level* g_current_level;

// ============================================================
// ADMIN COMMAND QUEUE (Thread-Safe)
// Stores commands from server to be processed in main thread
// ============================================================

#define ADMIN_QUEUE_SIZE 32

typedef struct {
    char command[512];
    bool valid;
} AdminCommand;

typedef struct {
    AdminCommand commands[ADMIN_QUEUE_SIZE];
    int head;
    int tail;
    pthread_mutex_t mutex;
} AdminCommandQueue;

static AdminCommandQueue g_admin_queue = {
    .head = 0,
    .tail = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

// ============================================================
// ADMIN COMMAND QUEUE FUNCTIONS
// ============================================================

static void admin_queue_enqueue(const char* command) {
    pthread_mutex_lock(&g_admin_queue.mutex);
    
    int next_tail = (g_admin_queue.tail + 1) % ADMIN_QUEUE_SIZE;
    
    // Check if queue is full
    if (next_tail == g_admin_queue.head) {
        printf("[ADMIN] WARNING: Command queue full, dropping: %s\n", command);
        pthread_mutex_unlock(&g_admin_queue.mutex);
        return;
    }
    
    // Add command to queue
    strncpy(g_admin_queue.commands[g_admin_queue.tail].command, command, 511);
    g_admin_queue.commands[g_admin_queue.tail].command[511] = '\0';
    g_admin_queue.commands[g_admin_queue.tail].valid = true;
    g_admin_queue.tail = next_tail;
    
    printf("[ADMIN] Command queued: %s\n", command);
    
    pthread_mutex_unlock(&g_admin_queue.mutex);
}

static bool admin_queue_dequeue(char* out_command, size_t max_len) {
    pthread_mutex_lock(&g_admin_queue.mutex);
    
    // Check if queue is empty
    if (g_admin_queue.head == g_admin_queue.tail) {
        pthread_mutex_unlock(&g_admin_queue.mutex);
        return false;
    }
    
    // Get command from queue
    AdminCommand* cmd = &g_admin_queue.commands[g_admin_queue.head];
    if (cmd->valid) {
        strncpy(out_command, cmd->command, max_len - 1);
        out_command[max_len - 1] = '\0';
        cmd->valid = false;
    }
    
    g_admin_queue.head = (g_admin_queue.head + 1) % ADMIN_QUEUE_SIZE;
    
    pthread_mutex_unlock(&g_admin_queue.mutex);
    return true;
}

// Structure to hold context for the listener thread
typedef struct {
    Connection* conn;
    volatile bool running;
    MessageCallback callback;
    void* user_data;
} MessageListenerContext;

static MessageListenerContext* g_listener_context = NULL;

static void* listener_thread_func(void* arg) {
    MessageListenerContext* ctx = (MessageListenerContext*)arg;
    char buffer[BUFFER_SIZE];
    
    printf("[LISTENER] Thread started\n");
    
    while (ctx->running && ctx->conn && ctx->conn->connected) {
        // Check if there's data available without blocking
        if (connection_has_data(ctx->conn)) {
            if (connection_receive(ctx->conn, buffer, BUFFER_SIZE)) {
                printf("[LISTENER] Received message: '%s'\n", buffer);
                
                // Check if this is an admin command
                if (strncmp(buffer, "SPAWN_", 6) == 0 || 
                    strncmp(buffer, "REMOVE_", 7) == 0) {
                    // Admin command → queue it for processing in main thread
                    admin_queue_enqueue(buffer);
                } else {
                    // Regular message → call callback
                    if (ctx->callback) {
                        printf("[LISTENER] Calling callback for message\n");
                        ctx->callback(buffer, ctx->user_data);
                        printf("[LISTENER] Callback completed\n");
                    } else {
                        printf("[LISTENER] WARNING: No callback set!\n");
                    }
                }
            } else {
                printf("[LISTENER] connection_receive returned false\n");
            }
        } else {
            usleep(100000);  // 100ms
        }
    }
    
    printf("[LISTENER] Thread stopped (running=%d, connected=%d)\n", 
           ctx->running, ctx->conn ? ctx->conn->connected : 0);
    return NULL;
}

pthread_t message_listener_start(Connection* conn) {
    if (!conn) {
        return 0;
    }
    
    MessageListenerContext* ctx = malloc(sizeof(MessageListenerContext));
    if (!ctx) {
        printf("Error: Could not allocate memory for message listener\n");
        return 0;
    }
    
    ctx->conn = conn;
    ctx->running = true;
    ctx->callback = NULL;
    ctx->user_data = NULL;
    g_listener_context = ctx;
    
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, listener_thread_func, ctx) != 0) {
        printf("Error: Could not create message listener thread\n");
        free(ctx);
        g_listener_context = NULL;
        return 0;
    }
    
    return thread_id;
}

void message_listener_set_callback(MessageCallback callback, void* user_data) {
    if (g_listener_context) {
        g_listener_context->callback = callback;
        g_listener_context->user_data = user_data;
    }
}

void message_listener_stop(pthread_t thread_id) {
    if (g_listener_context) {
        g_listener_context->running = false;
        
        if (thread_id != 0) {
            pthread_join(thread_id, NULL);
        }
        
        free(g_listener_context);
        g_listener_context = NULL;
    }
}

// ============================================================
// PUBLIC API FOR ADMIN COMMAND PROCESSING
// To be called from main thread (player_screen.c)
// ============================================================

void message_listener_process_admin_commands(struct Level* level, Connection* conn) {
    if (!level) {
        return;  // Can't process commands without a level
    }
    
    // Need to cast to Level* to access fields
    Level* lvl = (Level*)level;
    
    char command[512];
    
    // Process all queued commands (max 10 per frame to avoid lag)
    int processed = 0;
    while (processed < 10 && admin_queue_dequeue(command, sizeof(command))) {
        printf("[ADMIN] Processing command: %s\n", command);
        
        // ============================================================
        // FASE 2: SPAWN_ENEMY HANDLER
        // Format: "SPAWN_ENEMY:RED:5" or "SPAWN_ENEMY:BLUE:3"
        // ============================================================
        if (strncmp(command, ADMIN_SPAWN_ENEMY, strlen(ADMIN_SPAWN_ENEMY)) == 0) {
            // Parse command: "SPAWN_ENEMY:RED:5"
            const char* params = command + strlen(ADMIN_SPAWN_ENEMY);
            char type[32];
            int vine_id;
            
            if (sscanf(params, "%31[^:]:%d", type, &vine_id) == 2) {
                // Find empty enemy slot
                Enemy* empty_slot = NULL;
                for (int i = 0; i < lvl->max_enemies; i++) {
                    if (!lvl->enemies[i].active) {
                        empty_slot = &lvl->enemies[i];
                        break;
                    }
                }
                
                if (empty_slot) {
                    bool success = false;
                    
                    if (strcmp(type, ENEMY_TYPE_RED) == 0) {
                        success = enemy_spawn_red_at_vine_id(empty_slot, level, vine_id);
                    }
                    else if (strcmp(type, ENEMY_TYPE_BLUE) == 0) {
                        success = enemy_spawn_blue_at_vine_id(empty_slot, level, vine_id);
                    }
                    else {
                        printf("[ADMIN] Error: Unknown enemy type '%s'\n", type);
                    }
                    
                    if (success) {
                        printf("[ADMIN] ✓ Spawned %s enemy at vine %d\n", type, vine_id);
                    }
                } else {
                    printf("[ADMIN] Error: No empty enemy slots (max=%d)\n", lvl->max_enemies);
                }
            } else {
                printf("[ADMIN] Error: Invalid SPAWN_ENEMY format: %s\n", command);
            }
        }
        // ============================================================
        // FASE 3: SPAWN_FRUIT HANDLER
        // Format: "SPAWN_FRUIT:vineId:positionY:type:fruitId"
        // Example: "SPAWN_FRUIT:2:10:Mango- 800 pts:1001"
        // ============================================================
        else if (strncmp(command, ADMIN_SPAWN_FRUIT, strlen(ADMIN_SPAWN_FRUIT)) == 0) {
            // Parse command
            const char* params = command + strlen(ADMIN_SPAWN_FRUIT);
            int vine_id, position_y, fruit_id;
            char type_str[64];
            
            // Parse: vineId:positionY:type:fruitId
            int parsed = sscanf(params, "%d:%d:%63[^:]:%d", &vine_id, &position_y, type_str, &fruit_id);
            
            if (parsed == 4) {
                // Spawn fruit using admin function
                bool success = fruit_spawn_admin(lvl, fruit_id, vine_id, position_y, type_str);
                
                if (success) {
                    printf("[ADMIN] ✓ Spawned fruit ID=%d at vine %d, pos %d\\n", fruit_id, vine_id, position_y);
                } else {
                    printf("[ADMIN] Error: Failed to spawn fruit\\n");
                }
            } else {
                printf("[ADMIN] Error: Invalid SPAWN_FRUIT format: %s\\n", command);
            }
        }
        // ============================================================
        // FASE 4: REMOVE_FRUIT HANDLER
        // ============================================================
        // Format: "REMOVE_FRUIT:fruitId"
        // Example: "REMOVE_FRUIT:1001"
        else if (strncmp(command, ADMIN_REMOVE_FRUIT, strlen(ADMIN_REMOVE_FRUIT)) == 0) {
            const char* params = command + strlen(ADMIN_REMOVE_FRUIT);
            
            int fruit_id = atoi(params);
            
            if (fruit_id > 0) {
                Level* lvl = (Level*)g_current_level;
                if (lvl && fruit_remove_by_id(lvl, fruit_id)) {
                    printf("[ADMIN] ✓ Removed fruit ID=%d\n", fruit_id);
                } else {
                    printf("[ADMIN] Error: Failed to remove fruit ID=%d\n", fruit_id);
                }
            } else {
                printf("[ADMIN] Error: Invalid REMOVE_FRUIT format: %s\n", command);
            }
        }
        else {
            printf("[ADMIN] → Unknown command: %s\n", command);
        }
        
        processed++;
    }
    
    (void)conn;  // Will be used in Phase 2/3 for confirmations
}
