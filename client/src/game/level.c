#include "level.h"
#include "../rendering/sprite_manager.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================
// SIMPLE TEST LEVEL DATA
// ============================================================

// Single platform for testing
static Platform PLATFORMS[] = {
    {400, 550, 400}   // Shorter centered platform
};

// No columns for now
static Column COLUMNS[] = {
    // Empty
};

// Two vines for testing climbing - side by side for dual climbing
static Vine VINES[] = {
    {1, 500, 80, 500, true},   // Left vine
    {2, 580, 80, 500, true}    // Right vine - close together (80px apart)
};

#define PLATFORM_COUNT (sizeof(PLATFORMS) / sizeof(Platform))
#define COLUMN_COUNT (sizeof(COLUMNS) / sizeof(Column))
#define VINE_COUNT (sizeof(VINES) / sizeof(Vine))

// No goal for now
#define CAGE_X 0.0f
#define CAGE_Y 0.0f
#define MARIO_X 0.0f
#define MARIO_Y 0.0f

// ============================================================
// LEVEL CREATION
// ============================================================

Level* level_create(void) {
    Level* level = (Level*)malloc(sizeof(Level));
    
    // Water
    level->water_level = WATER_LEVEL;
    
    // Platforms
    level->platform_count = PLATFORM_COUNT;
    level->platforms = (Platform*)malloc(sizeof(Platform) * PLATFORM_COUNT);
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        level->platforms[i] = PLATFORMS[i];
    }
    
    // Columns
    level->column_count = COLUMN_COUNT;
    level->columns = (Column*)malloc(sizeof(Column) * COLUMN_COUNT);
    for (int i = 0; i < COLUMN_COUNT; i++) {
        level->columns[i] = COLUMNS[i];
    }
    
    // Vines
    level->vine_count = VINE_COUNT;
    level->vines = (Vine*)malloc(sizeof(Vine) * VINE_COUNT);
    for (int i = 0; i < VINE_COUNT; i++) {
        level->vines[i] = VINES[i];
    }
    
    // Goal
    level->cage_x = CAGE_X;
    level->cage_y = CAGE_Y;
    level->mario_x = MARIO_X;
    level->mario_y = MARIO_Y;
    
    printf("✓ Level created: %d platforms, %d columns, %d vines\n",
           level->platform_count, level->column_count, level->vine_count);
    
    return level;
}

// ============================================================
// RENDERING HELPERS
// ============================================================

static void render_water(Level* level) {
    SpriteSheet* water_sprite = sprite_manager_get(SPRITE_WATER);
    if (!water_sprite || !water_sprite->loaded) return;
    
    // Repeat water texture across bottom
    int water_width = water_sprite->frame_width;
    int water_height = water_sprite->frame_height;
    
    for (int x = 0; x < LEVEL_WIDTH; x += water_width * 3) {
        Rectangle source = {0, 0, water_width, water_height};
        Rectangle dest = {x, level->water_level, water_width * 3, water_height * 3};
        DrawTexturePro(water_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void render_platforms(Level* level) {
    SpriteSheet* platform_sprite = sprite_manager_get(SPRITE_PLATFORM);
    if (!platform_sprite || !platform_sprite->loaded) return;
    
    int tile_width = platform_sprite->frame_width;
    int tile_height = platform_sprite->frame_height;
    
    for (int i = 0; i < level->platform_count; i++) {
        Platform* p = &level->platforms[i];
        
        // Repeat platform texture to fill width
        for (int x = 0; x < p->width; x += tile_width * 4) {
            Rectangle source = {0, 0, tile_width, tile_height};
            Rectangle dest = {p->x + x, p->y, tile_width * 4, tile_height * 4};
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
        
        // Draw stem (repeated vertically from water to height)
        float y = level->water_level;
        while (y > (level->water_level - col->height)) {
            Rectangle source = {0, 0, stem_width, stem_height};
            Rectangle dest = {col->x, y - stem_height, stem_width, stem_height};
            DrawTexturePro(stem_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
            y -= stem_height;
        }
        
        // Draw grass on top
        float grass_y = level->water_level - col->height;
        Rectangle source = {0, 0, grass_sprite->frame_width, grass_height};
        Rectangle dest = {col->x, grass_y - grass_height, grass_sprite->frame_width, grass_height};
        DrawTexturePro(grass_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void render_vines(Level* level) {
    SpriteSheet* vine_sprite = sprite_manager_get(SPRITE_VINE);
    if (!vine_sprite || !vine_sprite->loaded) return;
    
    int vine_width = vine_sprite->frame_width;
    int vine_height = vine_sprite->frame_height;
    
    for (int i = 0; i < level->vine_count; i++) {
        Vine* v = &level->vines[i];
        
        // Only render visible vines
        if (!v->visible) continue;
        
        // Repeat vine texture from top to bottom
        for (float y = v->y_top; y < v->y_bottom; y += vine_height * 3) {
            Rectangle source = {0, 0, vine_width, vine_height};
            Rectangle dest = {v->x - vine_width * 1.5f, y, vine_width * 3, vine_height * 3};
            DrawTexturePro(vine_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
        }
    }
}

// ============================================================
// MAIN RENDER FUNCTION
// ============================================================

void level_render(Level* level) {
    // Render order (back to front):
    render_water(level);      // Layer 1: Water (back)
    render_columns(level);    // Layer 2: Columns
    render_platforms(level);  // Layer 3: Platforms
    render_vines(level);      // Layer 4: Vines
    // Layer 5: Cage and Mario (coming next)
    // Layer 6: Enemies and Fruits (later)
    // Layer 7: Player (later)
}

// ============================================================
// CLEANUP
// ============================================================

void level_destroy(Level* level) {
    if (level) {
        free(level->platforms);
        free(level->columns);
        free(level->vines);
        free(level);
        printf("✓ Level destroyed\n");
    }
}
