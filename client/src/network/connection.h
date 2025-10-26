#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdbool.h>

// Platform-specific socket type
#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

/**
 * Connection - TCP socket connection state
 */
typedef struct {
    socket_t socket_fd;  // Changed from int
    bool connected;
} Connection;

/**
 * Create connection to server
 * @param ip Server IP address (e.g. "127.0.0.1")
 * @param port Server port number
 * @return Connection pointer or NULL on failure
 */
Connection* connection_create(const char* ip, int port);

/**
 * Send message to server
 * Automatically appends newline
 * @param conn Connection pointer
 * @param message Null-terminated string to send
 * @return true on success, false on failure
 */
bool connection_send(Connection* conn, const char* message);

/**
 * Receive message from server (blocking)
 * Strips trailing newline
 * @param conn Connection pointer
 * @param buffer Buffer to store received data
 * @param buffer_size Size of buffer
 * @return Pointer to buffer on success, NULL on failure
 */
char* connection_receive(Connection* conn, char* buffer, int buffer_size);

/**
 * Close connection and free resources
 * @param conn Connection pointer to close
 */
void connection_close(Connection* conn);

#endif