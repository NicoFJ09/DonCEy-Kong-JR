#include "spectator_protocol.h"
#include "../utils/constants.h"
#include "../external/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <raylib.h>

bool parse_game_state_message(const char* message, GameStateSnapshot* snapshot) {
    if (!message || !snapshot) return false;
    
    // Verificar prefijo "GAME_STATE:"
    if (strncmp(message, PROTO_GAME_STATE, strlen(PROTO_GAME_STATE)) != 0) {
        return false;
    }
    
    // Extraer JSON
    const char* json_str = message + strlen(PROTO_GAME_STATE);
    
    cJSON* root = cJSON_Parse(json_str);
    if (!root) {
        printf("ERROR: Failed to parse game state JSON\n");
        return false;
    }
    
    // Parsear score, lives, level
    cJSON* score = cJSON_GetObjectItem(root, "score");
    cJSON* lives = cJSON_GetObjectItem(root, "lives");
    cJSON* level = cJSON_GetObjectItem(root, "level");
    
    if (score) snapshot->score = score->valueint;
    if (lives) snapshot->lives = lives->valueint;
    if (level) snapshot->level = level->valueint;
    
    // Parsear entidades
    cJSON* entities = cJSON_GetObjectItem(root, "entities");
    if (entities && cJSON_IsArray(entities)) {
        snapshot->entity_count = 0;
        
        cJSON* entity = NULL;
        cJSON_ArrayForEach(entity, entities) {
            if (snapshot->entity_count >= MAX_ENTITIES) break;
            
            EntitySnapshot* ent = &snapshot->entities[snapshot->entity_count];
            
            cJSON* id = cJSON_GetObjectItem(entity, "id");
            cJSON* type = cJSON_GetObjectItem(entity, "type");
            cJSON* x = cJSON_GetObjectItem(entity, "x");
            cJSON* y = cJSON_GetObjectItem(entity, "y");
            cJSON* state = cJSON_GetObjectItem(entity, "state");
            cJSON* active = cJSON_GetObjectItem(entity, "active");
            cJSON* direction = cJSON_GetObjectItem(entity, "direction");
            
            if (id) ent->id = id->valueint;
            if (type) ent->type = type->valueint;
            if (x) ent->x = (float)x->valuedouble;
            if (y) ent->y = (float)y->valuedouble;
            if (active) ent->active = cJSON_IsTrue(active);
            if (direction) ent->direction = direction->valueint;
            
            // Handle state based on entity type
            if (state) {
                if (ent->type == ENTITY_PLAYER) {
                    ent->player_state = state->valueint;
                } else if (ent->type == ENTITY_ENEMY_RED || ent->type == ENTITY_ENEMY_BLUE) {
                    ent->enemy_state = state->valueint;
                }
            }
            
            snapshot->entity_count++;
        }
    }
    
    snapshot->timestamp = GetTime();
    
    cJSON_Delete(root);
    return true;
}