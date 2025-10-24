#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // 1. Crear socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
        printf("No se pudo crear el socket\n");
        return 1;
    }

    // 2. Configurar dirección del server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(12345); // Puerto server 
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // IP server

    // 3. Conectar al servidor
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("No se pudo conectar\n");
        return 1;
    }
    printf("Conectado al servidor!\n");

    // 4. Enviar mensajes
    char buffer[1024];
    
    for (int i = 0; i < 5; i++) {
        sprintf(buffer, "Hola desde C, mensaje #%d\n", i);
        send(sock, buffer, strlen(buffer), 0);
        printf("Enviado: %s", buffer);
        
        // Recibir respuesta
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        buffer[bytes] = '\0';
        printf("Servidor responde: %s\n", buffer);
        
        sleep(1);  // Esperar 1 segundo
    }

    // 5. Cerrar conexión
    close(sock);
    printf("Desconectado\n");

    return 0;
}