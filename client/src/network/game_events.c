#include "game_events.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <string.h>

bool event_send_fruit_collected(Connection* conn, int fruit_id, int points) {
    if (!conn || !conn->connected) {
        printf("[EVENT] Cannot send FRUIT_COLLECTED - not connected\n");
        return false;
    }
    
    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "%s%d:%d", EVENT_FRUIT_COLLECTED, fruit_id, points);
    
    bool result = connection_send(conn, message);
    if (result) {
        printf("[EVENT] Sent FRUIT_COLLECTED (id=%d, points=%d)\n", fruit_id, points);
    } else {
        printf("[EVENT] Failed to send FRUIT_COLLECTED\n");
    }
    
    return result;
}

bool event_send_player_died(Connection* conn, const char* cause) {
    if (!conn || !conn->connected) {
        printf("[EVENT] Cannot send PLAYER_DIED - not connected\n");
        return false;
    }
    
    if (!cause) {
        cause = "unknown";
    }
    
    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "%s%s", EVENT_PLAYER_DIED, cause);
    
    bool result = connection_send(conn, message);
    if (result) {
        printf("[EVENT] Sent PLAYER_DIED (cause=%s)\n", cause);
    } else {
        printf("[EVENT] Failed to send PLAYER_DIED\n");
    }
    
    return result;
}

bool event_send_level_completed(Connection* conn) {
    if (!conn || !conn->connected) {
        printf("[EVENT] Cannot send LEVEL_COMPLETED - not connected\n");
        return false;
    }
    
    bool result = connection_send(conn, EVENT_LEVEL_COMPLETED);
    if (result) {
        printf("[EVENT] Sent LEVEL_COMPLETED\n");
    } else {
        printf("[EVENT] Failed to send LEVEL_COMPLETED\n");
    }
    
    return result;
}

bool event_send_player_respawn(Connection* conn) {
    if (!conn || !conn->connected) {
        printf("[EVENT] Cannot send PLAYER_RESPAWN - not connected\n");
        return false;
    }
    
    bool result = connection_send(conn, "PLAYER_RESPAWN");
    if (result) {
        printf("[EVENT] Sent PLAYER_RESPAWN\n");
    } else {
        printf("[EVENT] Failed to send PLAYER_RESPAWN\n");
    }
    
    return result;
}
