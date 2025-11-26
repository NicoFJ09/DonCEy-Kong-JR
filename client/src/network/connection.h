#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdbool.h>

// Platform-specific socket type
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #define NOUSER
    #include <winsock2.h>
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

/**
 * Connection - TCP socket connection state
 */
typedef struct {
    socket_t socket_fd;
    bool connected;
    int client_id;  // Server-assigned client ID
    double last_send_time;  // Last time a message was sent (for throttling)

    // Map data from server
    char* map_json;      // Allocated string copy of JSON map data
    bool map_loaded;     // Flag indicating JSON is ready for use
} Connection;

/**
 * Initialize network subsystem (Windows WSA)
 * Must be called before any connection operations
 * @return true on success, false on failure
 */
bool connection_init(void);

/**
 * Cleanup network subsystem (Windows WSA)
 * Should be called at program exit
 */
void connection_cleanup_global(void);

/**
 * Create connection to server
 * @param ip Server IP address (e.g. "127.0.0.1")
 * @param port Server port number
 * @return Connection pointer or NULL on failure
 */
Connection* connection_create(const char* ip, int port);

/**
 * Send message to server with throttling
 * Automatically appends newline and enforces minimum delay between sends
 * @param conn Connection pointer
 * @param message Null-terminated string to send
 * @return true on success, false on failure
 */
bool connection_send(Connection* conn, const char* message);

/**
 * Send message to server immediately without throttling
 * Use for critical commands (DISCONNECT, etc.)
 * @param conn Connection pointer
 * @param message Null-terminated string to send
 * @return true on success, false on failure
 */
bool connection_send_immediate(Connection* conn, const char* message);

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
 * Receive message from server with timeout
 * Strips trailing newline
 * @param conn Connection pointer
 * @param buffer Buffer to store received data
 * @param buffer_size Size of buffer
 * @param timeout_seconds Timeout in seconds (e.g., 3.0 for 3 seconds)
 * @return Pointer to buffer on success, NULL on timeout or failure
 */
char* connection_receive_with_timeout(Connection* conn, char* buffer, int buffer_size, double timeout_seconds);

/**
 * Check if there is data available to read (non-blocking)
 * @param conn Connection pointer
 * @return true if data is available, false otherwise
 */
bool connection_has_data(Connection* conn);

/**
 * Drain/clear all pending data from socket buffer
 * Use before reconnecting to ensure clean state
 * @param conn Connection pointer
 */
void connection_drain_buffer(Connection* conn);

/**
 * Close connection and free resources
 * @param conn Connection pointer to close
 */
void connection_close(Connection* conn);

#endif