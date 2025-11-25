#include "level.h"
#include "../rendering/sprite_manager.h"
#include "../utils/constants.h"
#include "../external/cJSON.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================
// LEVEL CREATION FROM SERVER JSON
// ============================================================

/**
 * Create level from server JSON
 * @param json_data JSON string (without "MAP_DATA:" prefix)
 * @return Level* or NULL on parse error
 */
Level* level_create_from_json(const char* json_data) {
    if (!json_data) {
        printf("ERROR: NULL JSON data\n");
        return NULL;
    }

    cJSON* root = cJSON_Parse(json_data);
    if (!root) {
        printf("ERROR: Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    Level* level = (Level*)malloc(sizeof(Level));
    if (!level) {
        printf("ERROR: Failed to allocate level\n");
        cJSON_Delete(root);
        return NULL;
    }

    cJSON* platforms_array = cJSON_GetObjectItem(root, "platforms");
    if (!platforms_array || !cJSON_IsArray(platforms_array)) {
        printf("ERROR: Missing or invalid 'platforms' array\n");
        free(level);
        cJSON_Delete(root);
        return NULL;
    }

    int platform_count = cJSON_GetArraySize(platforms_array);
    level->platform_count = platform_count;
    level->platforms = (Platform*)malloc(sizeof(Platform) * platform_count);

    for (int i = 0; i < platform_count; i++) {
        cJSON* p = cJSON_GetArrayItem(platforms_array, i);
        cJSON* xBlock = cJSON_GetObjectItem(p, "xBlock");
        cJSON* yBlock = cJSON_GetObjectItem(p, "yBlock");
        cJSON* widthBlocks = cJSON_GetObjectItem(p, "widthBlocks");

        if (!xBlock || !yBlock || !widthBlocks) {
            printf("ERROR: Invalid platform at index %d\n", i);
            free(level->platforms);
            free(level);
            cJSON_Delete(root);
            return NULL;
        }

        level->platforms[i].x = xBlock->valueint * PLATFORM_BLOCK_SIZE;
        level->platforms[i].y = yBlock->valueint * PLATFORM_BLOCK_SIZE;
        level->platforms[i].width_blocks = widthBlocks->valueint;
    }

    cJSON* columns_array = cJSON_GetObjectItem(root, "columns");
    if (!columns_array || !cJSON_IsArray(columns_array)) {
        printf("ERROR: Missing or invalid 'columns' array\n");
        free(level->platforms);
        free(level);
        cJSON_Delete(root);
        return NULL;
    }

    int column_count = cJSON_GetArraySize(columns_array);
    level->column_count = column_count;
    level->columns = (Column*)malloc(sizeof(Column) * column_count);

    for (int i = 0; i < column_count; i++) {
        cJSON* c = cJSON_GetArrayItem(columns_array, i);
        cJSON* xBlock = cJSON_GetObjectItem(c, "xBlock");
        cJSON* yBlock = cJSON_GetObjectItem(c, "yBlock");
        cJSON* wide = cJSON_GetObjectItem(c, "wide");

        if (!xBlock || !yBlock || !cJSON_IsBool(wide)) {
            printf("ERROR: Invalid column at index %d\n", i);
            free(level->columns);
            free(level->platforms);
            free(level);
            cJSON_Delete(root);
            return NULL;
        }

        int grass_width = cJSON_IsTrue(wide) ? 96 : 72;
        float grass_center_x = xBlock->valueint * PLATFORM_BLOCK_SIZE;
        level->columns[i].x = grass_center_x - (grass_width / 2.0f);
        level->columns[i].y = yBlock->valueint * PLATFORM_BLOCK_SIZE;
        level->columns[i].wide = cJSON_IsTrue(wide);
        level->columns[i].grass_width = grass_width;
    }

    cJSON* vine_groups_array = cJSON_GetObjectItem(root, "vineGroups");
    if (!vine_groups_array || !cJSON_IsArray(vine_groups_array)) {
        printf("ERROR: Missing or invalid 'vineGroups' array\n");
        free(level->columns);
        free(level->platforms);
        free(level);
        cJSON_Delete(root);
        return NULL;
    }

    int total_visible = 0;
    int vine_group_count = cJSON_GetArraySize(vine_groups_array);

    for (int g = 0; g < vine_group_count; g++) {
        cJSON* group = cJSON_GetArrayItem(vine_groups_array, g);
        cJSON* heights = cJSON_GetObjectItem(group, "heights");
        if (heights && cJSON_IsArray(heights)) {
            total_visible += cJSON_GetArraySize(heights);
        }
    }

    int total_center = 0;
    for (int g = 0; g < vine_group_count; g++) {
        cJSON* group = cJSON_GetArrayItem(vine_groups_array, g);
        cJSON* heights = cJSON_GetObjectItem(group, "heights");
        if (heights && cJSON_IsArray(heights)) {
            int vine_count = cJSON_GetArraySize(heights);
            if (vine_count > 1) {
                total_center += (vine_count - 1);
            }
        }
    }

    int total_vines = total_visible + total_center;
    level->vine_count = total_vines;
    level->vines = (Vine*)malloc(sizeof(Vine) * total_vines);

    int vine_index = 0;
    int next_id = 1;

    for (int g = 0; g < vine_group_count; g++) {
        cJSON* group = cJSON_GetArrayItem(vine_groups_array, g);
        cJSON* xBlock = cJSON_GetObjectItem(group, "xBlock");
        cJSON* heights = cJSON_GetObjectItem(group, "heights");

        if (!xBlock || !heights || !cJSON_IsArray(heights)) {
            printf("ERROR: Invalid vine group at index %d\n", g);
            free(level->vines);
            free(level->columns);
            free(level->platforms);
            free(level);
            cJSON_Delete(root);
            return NULL;
        }

        float group_x_pixels = xBlock->valueint * PLATFORM_BLOCK_SIZE;
        int vine_count = cJSON_GetArraySize(heights);

        for (int i = 0; i < vine_count; i++) {
            cJSON* vh = cJSON_GetArrayItem(heights, i);
            cJSON* yTopBlock = cJSON_GetObjectItem(vh, "yTopBlock");
            cJSON* heightBlocks = cJSON_GetObjectItem(vh, "heightBlocks");

            if (!yTopBlock || !heightBlocks) {
                printf("ERROR: Invalid vine height at group %d, vine %d\n", g, i);
                free(level->vines);
                free(level->columns);
                free(level->platforms);
                free(level);
                cJSON_Delete(root);
                return NULL;
            }

            float vine_y_top = yTopBlock->valueint * VINE_SPRITE_HEIGHT;
            float vine_y_bottom = vine_y_top + (heightBlocks->valueint * VINE_SPRITE_HEIGHT);

            Vine visible;
            visible.id = next_id++;
            visible.x = group_x_pixels + (i * VINE_SPACING);
            visible.y_top = vine_y_top;
            visible.y_bottom = vine_y_bottom;
            visible.visible = true;

            level->vines[vine_index] = visible;
            vine_index++;

            if (i < vine_count - 1) {
                cJSON* next_vh = cJSON_GetArrayItem(heights, i + 1);
                cJSON* next_yTopBlock = cJSON_GetObjectItem(next_vh, "yTopBlock");
                cJSON* next_heightBlocks = cJSON_GetObjectItem(next_vh, "heightBlocks");

                float next_y_top = next_yTopBlock->valueint * VINE_SPRITE_HEIGHT;
                float next_y_bottom = next_y_top + (next_heightBlocks->valueint * VINE_SPRITE_HEIGHT);

                float center_y_top = (vine_y_top > next_y_top) ? vine_y_top : next_y_top;
                float center_y_bottom = (vine_y_bottom < next_y_bottom) ? vine_y_bottom : next_y_bottom;

                Vine center;
                center.id = next_id++;
                center.x = group_x_pixels + (i * VINE_SPACING) + (VINE_SPACING / 2.0f);
                center.y_top = center_y_top;
                center.y_bottom = center_y_bottom;
                center.visible = false;

                level->vines[vine_index] = center;
                vine_index++;
            }
        }
    }

    level->water_level = WATER_LEVEL;
    
    // Find the highest platform (smallest Y value) for Mario and DK cage
    float highest_platform_y = LEVEL_HEIGHT;  // Start with bottom
    float highest_platform_x = 0;
    
    for (int i = 0; i < level->platform_count; i++) {
        if (level->platforms[i].y < highest_platform_y) {
            highest_platform_y = level->platforms[i].y;
            highest_platform_x = level->platforms[i].x;
        }
    }
    
    // Position DK cage and Mario on left side of highest platform
    // DK cage: 48x34 sprite scaled 3x = 144x102
    // Mario: 16x16 sprite scaled 3x = 48x48
    level->cage_x = highest_platform_x + 5;   // Almost at the left edge
    level->cage_y = highest_platform_y - 102; // Sit on platform (cage height = 102)
    level->mario_x = level->cage_x + 144;     // Right next to cage (cage width = 144)
    level->mario_y = highest_platform_y - 48; // On top of platform (mario height = 48)

    // Initialize enemies - enemies will be spawned dynamically during gameplay
    int max_enemies = 30;  // Increased to support both red and blue crocodiles spawning
    level->max_enemies = max_enemies;
    level->enemy_count = 0;
    level->enemies = (Enemy*)calloc(max_enemies, sizeof(Enemy));  // Use calloc to zero-initialize

    if (!level->enemies) {
        printf("ERROR: Failed to allocate memory for enemies\n");
        return level;
    }

    // No enemies spawned at level start - they spawn during gameplay
    level->blue_spawn_timer = 0.0f;  // Initialize blue crocodile spawn timer
    level->red_spawn_timer = 0.0f;   // Initialize red crocodile spawn timer

    printf("✓ Level initialized with dynamic enemy spawning (max: %d)\n", max_enemies);

    cJSON_Delete(root);

#if DEBUG_MODE
    printf("✓ Level created from JSON: %d platforms, %d columns, %d vines (%d groups), %d enemies\n",
           level->platform_count, level->column_count, level->vine_count, vine_group_count, level->enemy_count);
#endif

    return level;
}

// ============================================================
// RENDERING HELPERS
// ============================================================

static void render_water(Level* level) {
    SpriteSheet* water_sprite = sprite_manager_get(SPRITE_WATER);
    if (!water_sprite || !water_sprite->loaded) return;

    int water_width = water_sprite->frame_width;
    int water_height = water_sprite->frame_height;

    for (int x = 0; x < LEVEL_WIDTH; x += water_width) {
        Rectangle source = {0, 0, water_width, water_height};
        Rectangle dest = {x, level->water_level, water_width, water_height};
        DrawTexturePro(water_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void render_platforms(Level* level) {
    SpriteSheet* platform_sprite = sprite_manager_get(SPRITE_PLATFORM);
    if (!platform_sprite || !platform_sprite->loaded) return;

    for (int i = 0; i < level->platform_count; i++) {
        Platform* p = &level->platforms[i];

        Rectangle source = {0, 0,
                           platform_sprite->frame_width,
                           platform_sprite->frame_height};

        for (int block = 0; block < p->width_blocks; block++) {
            Rectangle dest = {
                p->x + (block * PLATFORM_BLOCK_SIZE),
                p->y,
                PLATFORM_BLOCK_SIZE,
                PLATFORM_BLOCK_SIZE
            };

            DrawTexturePro(platform_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
        }
    }
}

static void render_columns(Level* level) {
    SpriteSheet* stem = sprite_manager_get(SPRITE_STEM);
    SpriteSheet* wide_stem = sprite_manager_get(SPRITE_STEM_WIDE);
    SpriteSheet* grass = sprite_manager_get(SPRITE_GRASS);
    SpriteSheet* wide_grass = sprite_manager_get(SPRITE_GRASS_WIDE);

    for (int i = 0; i < level->column_count; i++) {
        Column* col = &level->columns[i];

        SpriteSheet* stem_sprite = col->wide ? wide_stem : stem;
        SpriteSheet* grass_sprite = col->wide ? wide_grass : grass;

        if (!stem_sprite || !stem_sprite->loaded) continue;
        if (!grass_sprite || !grass_sprite->loaded) continue;

        int stem_width = stem_sprite->frame_width;
        int stem_height = stem_sprite->frame_height;
        int grass_height = grass_sprite->frame_height;

        float stem_x = col->x + (col->grass_width / 2.0f) - (stem_width / 2.0f);
        float stem_start_y = col->y + 24;
        float stem_end_y = level->water_level;

        for (float y = stem_start_y; y < stem_end_y; y += stem_height) {
            Rectangle source = {0, 0, stem_width, stem_height};
            Rectangle dest = {stem_x, y, stem_width, stem_height};
            DrawTexturePro(stem_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
        }

        Rectangle source = {0, 0, grass_sprite->frame_width, grass_height};
        Rectangle dest = {col->x, col->y, grass_sprite->frame_width, grass_height};
        DrawTexturePro(grass_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void render_vines(Level* level) {
    SpriteSheet* vine1 = sprite_manager_get(SPRITE_VINE_1);
    SpriteSheet* vine2 = sprite_manager_get(SPRITE_VINE_2);
    SpriteSheet* vine3 = sprite_manager_get(SPRITE_VINE_3);

    if (!vine1 || !vine1->loaded || !vine2 || !vine2->loaded || !vine3 || !vine3->loaded) return;

    for (int i = 0; i < level->vine_count; i++) {
        Vine* v = &level->vines[i];

        if (!v->visible) continue;

        int segment_index = 0;
        for (float y = v->y_top; y < v->y_bottom; y += VINE_HEIGHT) {
            SpriteSheet* current_sprite;
            int sprite_variant = segment_index % 3;
            if (sprite_variant == 0) {
                current_sprite = vine1;
            } else if (sprite_variant == 1) {
                current_sprite = vine2;
            } else {
                current_sprite = vine3;
            }

            Rectangle source = {0, 0, 24, 24};
            Rectangle dest = {
                v->x - VINE_WIDTH / 2,
                y,
                VINE_WIDTH,
                VINE_HEIGHT
            };
            DrawTexturePro(current_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
            
            segment_index++;
        }
    }
}

// ============================================================
// PUBLIC RENDERING
// ============================================================

void level_render(Level* level) {
    render_water(level);
    render_columns(level);
    render_vines(level);
    render_platforms(level);
}

// ============================================================
// CLEANUP
// ============================================================

void level_destroy(Level* level) {
    if (level) {
        free(level->platforms);
        free(level->columns);
        free(level->vines);
        free(level->enemies);
        free(level);
#if DEBUG_MODE
        printf("✓ Level destroyed\n");
#endif
    }
}
