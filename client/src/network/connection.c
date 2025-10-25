#include "connection.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// === Connection Management ===

/**
 * Create and connect to server
 */
Connection* connection_create(const char* ip, int port) {
    // Allocate connection structure
    Connection* conn = malloc(sizeof(Connection));
    if (!conn) {
        printf("Error: Could not allocate memory for connection\n");
        return NULL;
    }
    
    // Create TCP socket
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->socket_fd == -1) {
        printf("Error: Could not create socket\n");
        free(conn);
        return NULL;
    }
    
    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert IP string to binary form
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Error: Invalid server IP address\n");
        close(conn->socket_fd);
        free(conn);
        return NULL;
    }
    
    // Attempt connection
    printf("Connecting to %s:%d...\n", ip, port);
    if (connect(conn->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: Could not connect to server\n");
        close(conn->socket_fd);
        free(conn);
        return NULL;
    }
    
    conn->connected = true;
    printf("✓ Connected to server!\n\n");
    
    return conn;
}

// === Communication ===

/**
 * Send message to server
 * Appends newline automatically
 */
bool connection_send(Connection* conn, const char* message) {
    if (!conn || !conn->connected) {
        return false;
    }
    
    // Append newline to message
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s\n", message);
    
    ssize_t sent = send(conn->socket_fd, buffer, strlen(buffer), 0);
    return sent > 0;
}

/**
 * Receive message from server (blocking)
 * Strips trailing newline from received data
 */
char* connection_receive(Connection* conn, char* buffer, int buffer_size) {
    if (!conn || !conn->connected) {
        return NULL;
    }
    
    // Block until data received
    ssize_t bytes = recv(conn->socket_fd, buffer, buffer_size - 1, 0);
    
    if (bytes <= 0) {
        // Connection closed or error
        conn->connected = false;
        return NULL;
    }
    
    buffer[bytes] = '\0';
    
    // Strip trailing newline if present
    if (bytes > 0 && buffer[bytes - 1] == '\n') {
        buffer[bytes - 1] = '\0';
    }
    
    return buffer;
}

// === Cleanup ===

/**
 * Close connection and free memory
 */
void connection_close(Connection* conn) {
    if (conn) {
        if (conn->connected) {
            close(conn->socket_fd);
            conn->connected = false;
        }
        free(conn);
    }
}