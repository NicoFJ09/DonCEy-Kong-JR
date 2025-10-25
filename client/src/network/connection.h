#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdbool.h>

typedef struct {
    int socket_fd;
    bool connected;
} Connection;

// Initialize connection to server
Connection* connection_create(const char* ip, int port);

// Send message to server
bool connection_send(Connection* conn, const char* message);

// Receive message from server (blocking)
char* connection_receive(Connection* conn, char* buffer, int buffer_size);

// Close connection
void connection_close(Connection* conn);

#endif