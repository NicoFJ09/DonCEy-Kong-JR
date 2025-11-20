#include "message_listener.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Structure to hold context for the listener thread
//This holds the connection and a running flag


typedef struct {
    Connection* conn;
    volatile bool running;
} MessageListenerContext;

static MessageListenerContext* g_listener_context = NULL;

static void* listener_thread_func(void* arg) {
    MessageListenerContext* ctx = (MessageListenerContext*)arg;
    char buffer[BUFFER_SIZE];
    
    printf("DEBUG: Message listener thread started\n");
    
    while (ctx->running && ctx->conn && ctx->conn->connected) {
        // Check if there's data available without blocking
        if (connection_has_data(ctx->conn)) {
            if (connection_receive(ctx->conn, buffer, BUFFER_SIZE)) {
                printf("CLIENT RECEIVED FROM SERVER: %s\n", buffer);
            }
        } else {
            usleep(100000);  // 100ms
        }
    }
    
    printf("DEBUG: Message listener thread stopped\n");
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
