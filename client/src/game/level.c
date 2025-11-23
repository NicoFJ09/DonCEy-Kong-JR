#include "level.h"
#include "../rendering/sprite_manager.h"
#include "../utils/constants.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================
// SIMPLE TEST LEVEL DATA
// ============================================================

// Platform definitions in BLOCKS (each block = 24px)
// Format: {x_block, y_block, width_blocks}
// Height is ALWAYS 1 block
typedef struct {
    int x_block;
    int y_block;
    int width_blocks;
} PlatformDef;

static PlatformDef PLATFORM_DEFS[] = {
    // Main map layout
    {0, 36, 15},   // Spawn platform: x=0 (left border), y=36 blocks (864px, right above water), 15 blocks wide
    {0, 9, 30},    // Top left platform: x=0 (left border), y=9 blocks (216px), 30 blocks wide
    {30, 10, 12},  // Top right platform: x=30 blocks (720px), y=10 blocks (240px, one block lower), 12 blocks wide
    {36, 20, 14},  // Right platform: x=38 blocks (912px), y=20 blocks (480px, 10 blocks below top right), 12 blocks wide (aligned to right)
    {12, 18, 6},   // Platform above third vine: x=12 blocks (288), y=18 blocks (432px), 6 blocks wide
    {12, 24, 9}    // Platform below (15 blocks below top): x=12 blocks (288px), y=24 blocks (576px), 9 blocks wide

    // Old test platforms (commented out)
    // {16, 23, 17},  // Bottom platform: x=16 blocks (384px), y=23 blocks (552px), 17 blocks wide
    // {16, 8, 17}    // Top platform: x=16 blocks (384px), y=3 blocks (72px), 17 blocks wide
};

// Column definitions in BLOCKS
// Format: {x_block, y_block, wide}
// x_block = X position in blocks (centered on grass)
// y_block = Y position in blocks (top of grass, stems extend down to water)
// wide = true for wide grass/stem (96px/48px), false for normal (72px/24px)
typedef struct {
    int x_block;
    int y_block;
    bool wide;
} ColumnDef;

static ColumnDef COLUMN_DEFS[] = {
    // Columns to the right of spawn platform (spawn platform ends at x=15)
    {27, 34, true},   // Wide column: 5 blocks right of spawn platform, 2 stems + grass (y=34)
    {32, 35, false},  // Normal column: after 1 block gap, 1 stem + grass (y=35)
    {37, 34, true},   // Wide column: after 1 block gap, 2 stems + grass (y=34)
    {43, 33, true}    // Wide column: after 1 block gap, 3 stems + grass (y=33)

    // Old test columns (commented out)
    // {10, 25, false},  // Normal column at x=10 blocks (240px), y=25 blocks (600px)
    // {40, 20, true},   // Wide column at x=40 blocks (960px), y=20 blocks (480px)
};

// ============================================================
// VINE GROUP SYSTEM
// ============================================================
// Spacing is ALWAYS VINE_SPACING (from constants.h)
// Heights use VINE BLOCKS (VINE_SPRITE_HEIGHT units) - matches visual representation!

// Individual vine height definition (in BLOCKS, not pixels)
typedef struct {
    int y_top_block;      // Top position in 24px blocks from top of screen
    int height_blocks;    // Height in 24px blocks (e.g., 10 blocks = 240px tall vine)
} VineHeight;



// Custom heights for group 1 - vines below top left platform
// Top platform is at block 9 (216px), vines start at block 10 (right below it)
// Heights in 24px blocks - each block = one vine segment
static VineHeight GROUP1_HEIGHTS[] = {
    {10, 24},  // Vine 1: starts at block 10 (240px), 24 blocks tall (576px)
    {10, 21},  // Vine 2: starts at block 10 (240px), 21 blocks tall (504px)
    {19, 15}   // Vine 3: starts at block 19 (456px), 15 blocks tall (360px)
};

static VineHeight GROUP2_HEIGHTS[] = {
    {10, 18}
};

static VineHeight GROUP3A_HEIGHTS[] = {
    {10, 9},
    {11, 9}
};

static VineHeight GROUP3B_HEIGHTS[] = {
    {11, 13},
    {21, 5},
    {21, 9},
    {6, 21},
    {6, 24}
};

// Vine group definitions in BLOCKS
// x_block will be converted to pixels, spacing is automatic (VINE_SPACING)
typedef struct {
    int x_block;                // X position in blocks (will be * 24)
    int vine_count;             // How many VISIBLE vines to create
    VineHeight* heights;        // Array of heights for each vine (NULL = all use default)
    int default_y_top_block;    // Default top block if heights array is NULL
    int default_height_blocks;  // Default height in blocks if heights array is NULL
} VineGroupDef;

static VineGroupDef VINE_GROUP_DEFS[] = {
    {8, 3, GROUP1_HEIGHTS, 0, 0},
    {23, 1, GROUP2_HEIGHTS, 0, 0},
    {28, 2, GROUP3A_HEIGHTS, 0, 0},
    {35, 5, GROUP3B_HEIGHTS, 0, 0}
};

#define PLATFORM_COUNT (sizeof(PLATFORM_DEFS) / sizeof(PlatformDef))
#define COLUMN_COUNT (sizeof(COLUMN_DEFS) / sizeof(ColumnDef))
#define VINE_GROUP_COUNT (sizeof(VINE_GROUP_DEFS) / sizeof(VineGroupDef))

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
    
    // Water - use constant (positioned at block 37, one block from bottom)
    level->water_level = WATER_LEVEL;

#if DEBUG_MODE
    printf("Water level set to: %.0f pixels (block %d of %d)\n",
           level->water_level, WATER_LEVEL_BLOCKS, LEVEL_HEIGHT / 24);
#endif
    
    // Platforms - convert from blocks to pixels
    level->platform_count = (int)PLATFORM_COUNT;
    level->platforms = (Platform*)malloc(sizeof(Platform) * PLATFORM_COUNT);
    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
        level->platforms[i].x = PLATFORM_DEFS[i].x_block * PLATFORM_BLOCK_SIZE;
        level->platforms[i].y = PLATFORM_DEFS[i].y_block * PLATFORM_BLOCK_SIZE;
        level->platforms[i].width_blocks = PLATFORM_DEFS[i].width_blocks;
    }

    // Columns - convert from blocks to pixels
    level->column_count = (int)COLUMN_COUNT;
    level->columns = (Column*)malloc(sizeof(Column) * COLUMN_COUNT);
    for (size_t i = 0; i < COLUMN_COUNT; i++) {
        // Grass sprites: normal=72px wide, wide=96px wide
        int grass_width = COLUMN_DEFS[i].wide ? 96 : 72;

        // X position: center the grass at x_block position
        // Grass is centered at x_block position
        float grass_center_x = COLUMN_DEFS[i].x_block * PLATFORM_BLOCK_SIZE;

        level->columns[i].x = grass_center_x - (grass_width / 2.0f);  // Top-left of grass
        level->columns[i].y = COLUMN_DEFS[i].y_block * PLATFORM_BLOCK_SIZE;  // Top of grass
        level->columns[i].wide = COLUMN_DEFS[i].wide;
        level->columns[i].grass_width = grass_width;
    }

    // Vines - Auto-generate from vine groups
    // First, calculate total vine count across all groups
    int total_visible = 0;
    for (size_t g = 0; g < VINE_GROUP_COUNT; g++) {
        total_visible += VINE_GROUP_DEFS[g].vine_count;
    }

    // Total vines = visible + center vines
    // Each group with N vines creates (N-1) center vines
    int total_center = 0;
    for (size_t g = 0; g < VINE_GROUP_COUNT; g++) {
        if (VINE_GROUP_DEFS[g].vine_count > 1) {
            total_center += (VINE_GROUP_DEFS[g].vine_count - 1);
        }
    }

    int total_vines = total_visible + total_center;
    level->vine_count = total_vines;
    level->vines = (Vine*)malloc(sizeof(Vine) * total_vines);

    int vine_index = 0;
    int next_id = 1;

    // Process each vine group
    for (size_t g = 0; g < VINE_GROUP_COUNT; g++) {
        VineGroupDef* group = &VINE_GROUP_DEFS[g];
        float group_x_pixels = group->x_block * PLATFORM_BLOCK_SIZE;  // Convert blocks to pixels

        // Generate vines for this group (spacing is ALWAYS VINE_SPACING)
        for (int i = 0; i < group->vine_count; i++) {
            // Get height for this vine (convert blocks to pixels)
            float vine_y_top, vine_y_bottom;
            if (group->heights != NULL) {
                // Use custom height - convert blocks to pixels
                vine_y_top = group->heights[i].y_top_block * VINE_SPRITE_HEIGHT;
                vine_y_bottom = vine_y_top + (group->heights[i].height_blocks * VINE_SPRITE_HEIGHT);
            } else {
                // Use default height - convert blocks to pixels
                vine_y_top = group->default_y_top_block * VINE_SPRITE_HEIGHT;
                vine_y_bottom = vine_y_top + (group->default_height_blocks * VINE_SPRITE_HEIGHT);
            }

            // Add visible vine
            Vine visible;
            visible.id = next_id++;
            visible.x = group_x_pixels + (i * VINE_SPACING);
            visible.y_top = vine_y_top;
            visible.y_bottom = vine_y_bottom;
            visible.visible = true;

            level->vines[vine_index] = visible;
            vine_index++;

            // Add center vine between this and next visible vine (if not last in group)
            if (i < group->vine_count - 1) {
                // Get next vine's height (convert blocks to pixels)
                float next_y_top, next_y_bottom;
                if (group->heights != NULL) {
                    next_y_top = group->heights[i + 1].y_top_block * VINE_SPRITE_HEIGHT;
                    next_y_bottom = next_y_top + (group->heights[i + 1].height_blocks * VINE_SPRITE_HEIGHT);
                } else {
                    next_y_top = group->default_y_top_block * VINE_SPRITE_HEIGHT;
                    next_y_bottom = next_y_top + (group->default_height_blocks * VINE_SPRITE_HEIGHT);
                }

                // CENTER VINE RULE:
                // centerTop = max(leftTop, rightTop) - lowest starting point
                // centerBottom = min(leftBottom, rightBottom) - highest ending point
                // This ensures center only exists where BOTH vines overlap
                float center_y_top = (vine_y_top > next_y_top) ? vine_y_top : next_y_top;
                float center_y_bottom = (vine_y_bottom < next_y_bottom) ? vine_y_bottom : next_y_bottom;

                Vine center;
                center.id = next_id++;
                center.x = group_x_pixels + (i * VINE_SPACING) + (VINE_SPACING / 2.0f);  // Midpoint
                center.y_top = center_y_top;
                center.y_bottom = center_y_bottom;
                center.visible = false;  // INVISIBLE bridge

                level->vines[vine_index] = center;
                vine_index++;
            }
        }
    }
    
    // Goal
    level->cage_x = CAGE_X;
    level->cage_y = CAGE_Y;
    level->mario_x = MARIO_X;
    level->mario_y = MARIO_Y;

#if DEBUG_MODE
    printf("✓ Level created: %d platforms, %d columns, %d vines (%d groups)\n",
           level->platform_count, level->column_count, level->vine_count, (int)VINE_GROUP_COUNT);
    printf("  Vine groups:\n");
    for (size_t g = 0; g < VINE_GROUP_COUNT; g++) {
        VineGroupDef* vg = &VINE_GROUP_DEFS[g];
        printf("    Group %zu: %d vines at x=%d blocks (%.0fpx), spacing=%.0fpx\n",
               g+1, vg->vine_count, vg->x_block, vg->x_block * 24.0f, VINE_SPACING);
    }
    printf("  Complete vine layout:\n");
    for (int i = 0; i < level->vine_count; i++) {
        Vine* v = &level->vines[i];
        printf("    Vine %d: x=%.0f, y=%.0f-%.0f, %s\n",
               v->id, v->x, v->y_top, v->y_bottom, v->visible ? "VISIBLE" : "CENTER (invisible)");
    }
#endif

    return level;
}

// ============================================================
// RENDERING HELPERS
// ============================================================

static void render_water(Level* level) {
    SpriteSheet* water_sprite = sprite_manager_get(SPRITE_WATER);
    if (!water_sprite || !water_sprite->loaded) return;

    // Tile water texture horizontally at water level (single row)
    int water_width = water_sprite->frame_width;   // 30px
    int water_height = water_sprite->frame_height; // 24px

    // Tile horizontally only - single row at water level
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

        // Draw individual blocks horizontally (tiled, not scaled)
        // Each block is PLATFORM_BLOCK_SIZE × PLATFORM_BLOCK_SIZE (24x24)
        Rectangle source = {0, 0,
                           platform_sprite->frame_width,   // 24
                           platform_sprite->frame_height}; // 24

        // Draw each block side-by-side
        for (int block = 0; block < p->width_blocks; block++) {
            Rectangle dest = {
                p->x + (block * PLATFORM_BLOCK_SIZE),  // Offset each block horizontally
                p->y,
                PLATFORM_BLOCK_SIZE,  // 24px width
                PLATFORM_BLOCK_SIZE   // 24px height
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

        int stem_width = stem_sprite->frame_width;   // 24 or 48
        int stem_height = stem_sprite->frame_height; // 24
        int grass_height = grass_sprite->frame_height; // 24

        // Draw stems vertically from 24px below grass down to water level
        // Stems are centered under the grass
        float stem_x = col->x + (col->grass_width / 2.0f) - (stem_width / 2.0f);
        float stem_start_y = col->y + 24; // Start 24px below grass
        float stem_end_y = level->water_level; // End directly at water level

        for (float y = stem_start_y; y < stem_end_y; y += stem_height) {
            Rectangle source = {0, 0, stem_width, stem_height};
            Rectangle dest = {stem_x, y, stem_width, stem_height};
            DrawTexturePro(stem_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
        }

        // Draw grass on top at column position
        Rectangle source = {0, 0, grass_sprite->frame_width, grass_height};
        Rectangle dest = {col->x, col->y, grass_sprite->frame_width, grass_height};
        DrawTexturePro(grass_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

static void render_vines(Level* level) {
    // Load all 3 vine sprite variants
    SpriteSheet* vine1 = sprite_manager_get(SPRITE_VINE_1);
    SpriteSheet* vine2 = sprite_manager_get(SPRITE_VINE_2);
    SpriteSheet* vine3 = sprite_manager_get(SPRITE_VINE_3);
    
    if (!vine1 || !vine1->loaded || !vine2 || !vine2->loaded || !vine3 || !vine3->loaded) return;

    for (int i = 0; i < level->vine_count; i++) {
        Vine* v = &level->vines[i];

        // Only render visible vines
        if (!v->visible) continue;

        // Stack vine segments from top to bottom (24px per block)
        // Alternate between vine1, vine2, vine3 for visual variety
        int segment_index = 0;
        for (float y = v->y_top; y < v->y_bottom; y += VINE_HEIGHT) {
            // Cycle through vine sprites: 1 → 2 → 3 → 1 → 2 → 3...
            SpriteSheet* current_sprite;
            int sprite_variant = segment_index % 3;
            if (sprite_variant == 0) {
                current_sprite = vine1;
            } else if (sprite_variant == 1) {
                current_sprite = vine2;
            } else {
                current_sprite = vine3;
            }

            Rectangle source = {0, 0, 24, 24};  // Native 24×24px sprite
            Rectangle dest = {
                v->x - VINE_WIDTH / 2,  // Center vine on x position
                y,
                VINE_WIDTH,   // 24px width (no scaling)
                VINE_HEIGHT   // 24px height (no scaling)
            };
            DrawTexturePro(current_sprite->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
            
            segment_index++;
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
    render_vines(level);      // Layer 3: Vines
    render_platforms(level);  // Layer 4: Platforms (always on top of vines)
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
#if DEBUG_MODE
        printf("✓ Level destroyed\n");
#endif
    }
}
