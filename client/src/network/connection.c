#include "connection.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

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
    printf("Connected to server!\n\n");
    
    return conn;
}

bool connection_send(Connection* conn, const char* message) {
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

char* connection_receive(Connection* conn, char* buffer, int buffer_size) {
    if (!conn || !conn->connected) {
        return NULL;
    }
    
    #ifdef _WIN32
    int bytes = recv(conn->socket_fd, buffer, buffer_size - 1, 0);
    #else
    ssize_t bytes = recv(conn->socket_fd, buffer, buffer_size - 1, 0);
    #endif
    
    if (bytes <= 0) {
        conn->connected = false;
        return NULL;
    }
    
    buffer[bytes] = '\0';
    
    if (bytes > 0 && buffer[bytes - 1] == '\n') {
        buffer[bytes - 1] = '\0';
    }
    
    return buffer;
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