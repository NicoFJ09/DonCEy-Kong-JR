#include "connection.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/select.h>
#endif

// Minimum delay between sends in seconds (prevents server spam)
#define SEND_THROTTLE_DELAY 0.05  // 50ms = max 20 messages/second

// Buffer persistente para acumular datos entre llamadas de receive
// Cada conexión necesita su propio buffer, indexado por socket_fd
static char g_line_buffers[64][BUFFER_SIZE * 2] = {0};  // Hasta 64 conexiones

bool connection_init(void) {
    #ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error: WSAStartup failed\n");
        return false;
    }
    #endif
    return true;
}

void connection_cleanup_global(void) {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

Connection* connection_create(const char* ip, int port) {
    Connection* conn = malloc(sizeof(Connection));
    if (!conn) {
        printf("Error: Could not allocate memory for connection\n");
        return NULL;
    }
    
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    #ifdef _WIN32
    if (conn->socket_fd == INVALID_SOCKET) {
    #else
    if (conn->socket_fd == -1) {
    #endif
        printf("Error: Could not create socket\n");
        free(conn);
        return NULL;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Error: Invalid server IP address\n");
        #ifdef _WIN32
        closesocket(conn->socket_fd);
        #else
        close(conn->socket_fd);
        #endif
        free(conn);
        return NULL;
    }
    
    printf("Connecting to %s:%d...\n", ip, port);
    if (connect(conn->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: Could not connect to server\n");
        #ifdef _WIN32
        closesocket(conn->socket_fd);
        #else
        close(conn->socket_fd);
        #endif
        free(conn);
        return NULL;
    }
    
    conn->connected = true;
    conn->client_id = -1;
    conn->last_send_time = 0.0;
    printf("Connected to server!\n\n");
    
    return conn;
}

// Internal send function - actually sends data over socket
static bool connection_send_internal(Connection* conn, const char* message) {
    if (!conn || !conn->connected) {
        return false;
    }
    
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s\n", message);
    
    #ifdef _WIN32
    int sent = send(conn->socket_fd, buffer, (int)strlen(buffer), 0);
    #else
    ssize_t sent = send(conn->socket_fd, buffer, strlen(buffer), 0);
    #endif
    
    if (sent <= 0) {
        conn->connected = false;
        return false;
    }
    
    return true;
}

bool connection_send(Connection* conn, const char* message) {
    if (!conn || !conn->connected) {
        return false;
    }
    
    // Throttle: enforce minimum delay between sends
    double current_time = GetTime();
    double time_since_last = current_time - conn->last_send_time;
    
    if (time_since_last < SEND_THROTTLE_DELAY) {
        // Wait for the remaining time
        double wait_time = SEND_THROTTLE_DELAY - time_since_last;
        WaitTime(wait_time);
    }
    
    bool result = connection_send_internal(conn, message);
    if (result) {
        conn->last_send_time = GetTime();
    }
    
    return result;
}

bool connection_send_immediate(Connection* conn, const char* message) {
    // Send without throttling - for critical commands
    bool result = connection_send_internal(conn, message);
    if (result) {
        conn->last_send_time = GetTime();
    }
    return result;
}

char* connection_receive(Connection* conn, char* buffer, int buffer_size) {
    if (!conn || !conn->connected) {
        return NULL;
    }
    
    // Seleccionar el buffer correcto para esta conexión
    int buf_idx = conn->socket_fd % 64;
    char* line_buffer = g_line_buffers[buf_idx];
    
    // Buscar línea completa en el buffer existente
    char* newline = strchr(line_buffer, '\n');
    
    if (newline) {
        // Ya hay una línea completa en el buffer
        *newline = '\0';
        
        // Copiar la línea al buffer de salida
        strncpy(buffer, line_buffer, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        
        // Remover \r si existe
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
        }
        
        // Mover el resto del buffer hacia adelante
        memmove(line_buffer, newline + 1, strlen(newline + 1) + 1);
        
        return buffer;
    }
    
    // No hay línea completa, necesitamos leer más datos del socket
    int current_len = strlen(line_buffer);
    int remaining_space = (BUFFER_SIZE * 2) - current_len - 1;
    
    if (remaining_space <= 0) {
        // Buffer lleno sin línea completa - error de protocolo
        printf("ERROR: Line buffer overflow, resetting\n");
        line_buffer[0] = '\0';
        return NULL;
    }
    
    #ifdef _WIN32
    int bytes = recv(conn->socket_fd, line_buffer + current_len, remaining_space, 0);
    #else
    ssize_t bytes = recv(conn->socket_fd, line_buffer + current_len, remaining_space, 0);
    #endif
    
    if (bytes <= 0) {
        conn->connected = false;
        return NULL;
    }
    
    line_buffer[current_len + bytes] = '\0';
    
    // Buscar línea completa en el buffer actualizado
    newline = strchr(line_buffer, '\n');
    if (!newline) {
        // Todavía no hay línea completa
        return NULL;
    }
    
    *newline = '\0';
    
    // Copiar la línea al buffer de salida
    strncpy(buffer, line_buffer, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    
    // Remover \r si existe
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\r') {
        buffer[len - 1] = '\0';
    }
    
    // Mover el resto del buffer hacia adelante
    memmove(line_buffer, newline + 1, strlen(newline + 1) + 1);
    
    return buffer;
}

char* connection_receive_with_timeout(Connection* conn, char* buffer, int buffer_size, double timeout_seconds) {
    if (!conn || !conn->connected) {
        return NULL;
    }
    
    // Primero verificar si hay una línea completa en el buffer persistente
    int buf_idx = conn->socket_fd % 64;
    char* line_buffer = g_line_buffers[buf_idx];
    
    // Si hay una línea completa en el buffer, devolverla inmediatamente
    char* newline = strchr(line_buffer, '\n');
    if (newline) {
        // Usar connection_receive() que maneja correctamente el buffer
        return connection_receive(conn, buffer, buffer_size);
    }
    
    #ifdef _WIN32
    // Windows: Use select with timeout (solo si buffer está vacío)
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(conn->socket_fd, &read_fds);
    
    timeout.tv_sec = (long)timeout_seconds;
    timeout.tv_usec = (long)((timeout_seconds - (long)timeout_seconds) * 1000000);
    
    int result = select(0, &read_fds, NULL, NULL, &timeout);
    
    if (result > 0) {
        // Data available - read normally
        return connection_receive(conn, buffer, buffer_size);
    } else if (result == 0) {
        // Timeout
        printf("DEBUG: Receive timeout after %.1fs\n", timeout_seconds);
        return NULL;
    } else {
        // Error
        conn->connected = false;
        return NULL;
    }
    #else
    // Unix/Mac: Use select with timeout
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(conn->socket_fd, &read_fds);
    
    timeout.tv_sec = (long)timeout_seconds;
    timeout.tv_usec = (long)((timeout_seconds - (long)timeout_seconds) * 1000000);
    
    int result = select(conn->socket_fd + 1, &read_fds, NULL, NULL, &timeout);
    
    if (result > 0) {
        // Data available - read normally
        return connection_receive(conn, buffer, buffer_size);
    } else if (result == 0) {
        // Timeout
        printf("DEBUG: Receive timeout after %.1fs\n", timeout_seconds);
        return NULL;
    } else {
        // Error
        conn->connected = false;
        return NULL;
    }
    #endif
}

bool connection_has_data(Connection* conn) {
    if (!conn || !conn->connected) {
        return false;
    }
    
    #ifdef _WIN32
    u_long available = 0;
    ioctlsocket(conn->socket_fd, FIONREAD, &available);
    return available > 0;
    #else
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(conn->socket_fd, &read_fds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    int result = select(conn->socket_fd + 1, &read_fds, NULL, NULL, &timeout);
    return result > 0;
    #endif
}

void connection_close(Connection* conn) {
    if (conn) {
        if (conn->connected) {
            #ifdef _WIN32
            closesocket(conn->socket_fd);
            // CRITICAL FIX: DO NOT call WSACleanup() here!
            // It's called once at program exit via connection_cleanup_global()
            #else
            close(conn->socket_fd);
            #endif
            conn->connected = false;
        }
        free(conn);
    }
}