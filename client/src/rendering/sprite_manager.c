#include "sprite_manager.h"
#include <stdio.h>
#include <string.h>

static SpriteManager manager = {0};

/*
 * =================================================================
 * SPRITE CONFIGURATION
 * 
 * Format: {path, frame_width, frame_height, frame_count, spacing}
 * 
 * spacing = pixels between frames (usually 2 these sprites)
 * =================================================================
 */
static const struct {
    const char* path;
    int frame_width;
    int frame_height;
    int frame_count;
    int spacing;  // 2-pixel gap between frames
} SPRITE_CONFIG[SPRITE_COUNT] = {
    // Characters - Junior (16x16 frames with 2px spacing)
    [SPRITE_JUNIOR_IDLE]           = {"characters/junior/idle.png", 32, 16, 1, 0},
    [SPRITE_JUNIOR_RUN_LEFT]       = {"characters/junior/run_left.png", 32, 16, 3, 2},
    [SPRITE_JUNIOR_RUN_RIGHT]      = {"characters/junior/run_right.png", 32, 16, 3, 2},
    [SPRITE_JUNIOR_JUMP_LEFT]      = {"characters/junior/jump_left.png", 32, 16, 1, 0},
    [SPRITE_JUNIOR_JUMP_RIGHT]     = {"characters/junior/jump_right.png", 32, 16, 1, 0},
    [SPRITE_JUNIOR_CLIMB_LEFT]     = {"characters/junior/climb_left.png", 32, 16, 2, 2},
    [SPRITE_JUNIOR_CLIMB_RIGHT]    = {"characters/junior/climb_right.png", 32, 16, 2, 2},
    [SPRITE_JUNIOR_CLIMB_CENTER] = {"characters/junior/climb_center.png", 32, 16, 2, 2},
    [SPRITE_JUNIOR_DEATH]          = {"characters/junior/death.png", 32, 16, 2, 2},
    
    // Characters - DK and Mario
    [SPRITE_DK_CAGED]      = {"characters/donkey_kong/caged.png", 48, 34, 7, 2},
    [SPRITE_MARIO_STARE]   = {"characters/mario/stare.png", 16, 16, 2, 2},
    
    // Enemies - Red Snapjaw (16x16 with 2px spacing)
    [SPRITE_SNAPJAW_RED_UP]    = {"enemies/red_snapjaw/r_up.png", 16, 16, 2, 2},
    [SPRITE_SNAPJAW_RED_DOWN]  = {"enemies/red_snapjaw/r_down.png", 16, 16, 2, 2},
    [SPRITE_SNAPJAW_RED_LEFT]  = {"enemies/red_snapjaw/r_left.png", 16, 16, 2, 2},
    [SPRITE_SNAPJAW_RED_RIGHT] = {"enemies/red_snapjaw/r_right.png", 16, 16, 2, 2},
    
    // Enemies - Blue Snapjaw
    [SPRITE_SNAPJAW_BLUE_DOWN]  = {"enemies/blue_snapjaw/b_down.png", 16, 16, 2, 2},
    [SPRITE_SNAPJAW_BLUE_LEFT]  = {"enemies/blue_snapjaw/b_left.png", 16, 16, 2, 2},
    [SPRITE_SNAPJAW_BLUE_RIGHT] = {"enemies/blue_snapjaw/b_right.png", 16, 16, 2, 2},
    
    // Environment
    [SPRITE_VINE]       = {"environment/vines/vine.png", 24, 24, 1, 0},
    [SPRITE_PLATFORM]   = {"environment/platforms/platform.png", 24, 24, 1, 0},
    [SPRITE_WATER]      = {"environment/water/water.png", 10, 8, 1, 0},
    [SPRITE_GRASS]      = {"environment/grass/grass.png", 24, 8, 1, 0},
    [SPRITE_GRASS_WIDE] = {"environment/grass/wide_grass.png", 32, 8, 1, 0},
    [SPRITE_STEM]       = {"environment/grass/stem.png", 8, 8, 1, 0},
    [SPRITE_STEM_WIDE]  = {"environment/grass/wide_stem.png", 16, 8, 1, 0},
    
    // Fruit (16x16 single frames)
    [SPRITE_FRUIT_APPLE]  = {"fruit/apple.png", 16, 16, 1, 0},
    [SPRITE_FRUIT_BANANA] = {"fruit/banana.png", 16, 16, 1, 0},
    [SPRITE_FRUIT_MANGO]  = {"fruit/mango.png", 16, 16, 1, 0},
    
    // Effects
    [SPRITE_SPLASH]       = {"effects/splash/splash.png", 16, 16, 2, 2},
    [SPRITE_POINTS_200]   = {"effects/points/200.png", 16, 16, 1, 0},
    [SPRITE_POINTS_400]   = {"effects/points/400.png", 16, 16, 1, 0},
    [SPRITE_POINTS_800]   = {"effects/points/800.png", 16, 16, 1, 0},
    
    // UI
    [SPRITE_LIFE] = {"ui/lives/life.png", 16, 16, 1, 0},
    [SPRITE_LOGO] = {"ui/logo/logo.png", 310, 84, 1, 0},
};

bool sprite_manager_init(const char* assets_path) {
    if (manager.initialized) {
        printf("Warning: Sprite manager already initialized\n");
        return true;
    }
    
    printf("========================================\n");
    printf("Initializing sprite manager...\n");
    printf("Assets path: %s\n", assets_path);
    printf("========================================\n");
    
    int loaded_count = 0;
    
    for (int i = 0; i < SPRITE_COUNT; i++) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s%s", assets_path, SPRITE_CONFIG[i].path);
        
        printf("Loading %s... ", SPRITE_CONFIG[i].path);
        manager.sprites[i].texture = LoadTexture(full_path);
        
        if (manager.sprites[i].texture.id == 0) {
            printf("FAILED\n");
            manager.sprites[i].loaded = false;
        } else {
            printf("OK (%dx%d)\n", 
                   manager.sprites[i].texture.width,
                   manager.sprites[i].texture.height);
            
            manager.sprites[i].frame_width = SPRITE_CONFIG[i].frame_width;
            manager.sprites[i].frame_height = SPRITE_CONFIG[i].frame_height;
            manager.sprites[i].frame_count = SPRITE_CONFIG[i].frame_count;
            manager.sprites[i].spacing = SPRITE_CONFIG[i].spacing;
            manager.sprites[i].loaded = true;
            loaded_count++;
        }
    }
    
    manager.initialized = true;
    
    printf("========================================\n");
    printf("✓ Loaded %d/%d sprites\n", loaded_count, SPRITE_COUNT);
    printf("========================================\n\n");
    
    return loaded_count > 0;
}

SpriteSheet* sprite_manager_get(SpriteType type) {
    if (type < 0 || type >= SPRITE_COUNT) return NULL;
    if (!manager.sprites[type].loaded) return NULL;
    return &manager.sprites[type];
}

/*
 * =================================================================
 * DRAW FRAME - WITH 2-PIXEL SPACING SUPPORT
 * 
 * CRITICAL: Accounts for spacing between frames!
 * 
 * Formula: x_position = frame * (frame_width + spacing)
 * 
 * Example with 2px spacing:
 *   Frame 0: x = 0 * (16 + 2) = 0
 *   Frame 1: x = 1 * (16 + 2) = 18  (16 + 2 pixel gap)
 *   Frame 2: x = 2 * (16 + 2) = 36  (16 + 2 + 16 + 2)
 * =================================================================
 */
void sprite_manager_draw_frame(SpriteType type, int frame, float x, float y, float scale, Color tint) {
    SpriteSheet* sprite = sprite_manager_get(type);
    if (!sprite) return;
    
    // Clamp frame to valid range
    if (frame < 0) frame = 0;
    if (frame >= sprite->frame_count) frame = sprite->frame_count - 1;
    
    /*
     * CALCULATE SOURCE RECTANGLE
     * Add spacing between frames!
     */
    Rectangle source = {
        (float)(frame * (sprite->frame_width + sprite->spacing)),  // X with spacing!
        0.0f,
        (float)sprite->frame_width,
        (float)sprite->frame_height
    };
    
    Rectangle dest = {
        x, y,
        sprite->frame_width * scale,
        sprite->frame_height * scale
    };
    
    Vector2 origin = {0, 0};
    DrawTexturePro(sprite->texture, source, dest, origin, 0.0f, tint);
}

void sprite_manager_cleanup(void) {
    if (!manager.initialized) return;
    
    printf("Cleaning up sprite manager...\n");
    for (int i = 0; i < SPRITE_COUNT; i++) {
        if (manager.sprites[i].loaded) {
            UnloadTexture(manager.sprites[i].texture);
            manager.sprites[i].loaded = false;
        }
    }
    manager.initialized = false;
    printf("✓ Sprite manager cleanup complete\n");
}

/*
 * =================================================================
 * GET SPRITE NAME
 * Uses SWITCH statement for explicit, safe mapping
 * =================================================================
 */
const char* sprite_manager_get_name(SpriteType type) {
    switch (type) {
        // Characters - Junior
        case SPRITE_JUNIOR_IDLE:           return "Junior Idle";
        case SPRITE_JUNIOR_RUN_LEFT:       return "Junior Run L";
        case SPRITE_JUNIOR_RUN_RIGHT:      return "Junior Run R";
        case SPRITE_JUNIOR_JUMP_LEFT:      return "Junior Jump L";
        case SPRITE_JUNIOR_JUMP_RIGHT:     return "Junior Jump R";
        case SPRITE_JUNIOR_CLIMB_LEFT:     return "Junior Climb L";
        case SPRITE_JUNIOR_CLIMB_RIGHT:    return "Junior Climb R";
        case SPRITE_JUNIOR_CLIMB_CENTER: return "Junior Climb Center";
        case SPRITE_JUNIOR_DEATH:          return "Junior Death";
        
        // Characters - Others
        case SPRITE_DK_CAGED:    return "DK Caged";
        case SPRITE_MARIO_STARE: return "Mario Stare";
        
        // Enemies - Red Snapjaw
        case SPRITE_SNAPJAW_RED_UP:    return "Red Up";
        case SPRITE_SNAPJAW_RED_DOWN:  return "Red Down";
        case SPRITE_SNAPJAW_RED_LEFT:  return "Red Left";
        case SPRITE_SNAPJAW_RED_RIGHT: return "Red Right";
        
        // Enemies - Blue Snapjaw
        case SPRITE_SNAPJAW_BLUE_DOWN:  return "Blue Down";
        case SPRITE_SNAPJAW_BLUE_LEFT:  return "Blue Left";
        case SPRITE_SNAPJAW_BLUE_RIGHT: return "Blue Right";
        
        // Environment
        case SPRITE_VINE:       return "Vine";
        case SPRITE_PLATFORM:   return "Platform";
        case SPRITE_WATER:      return "Water";
        case SPRITE_GRASS:      return "Grass";
        case SPRITE_GRASS_WIDE: return "Wide Grass";
        case SPRITE_STEM:       return "Stem";
        case SPRITE_STEM_WIDE:  return "Wide Stem";
        
        // Fruit
        case SPRITE_FRUIT_APPLE:  return "Apple";
        case SPRITE_FRUIT_BANANA: return "Banana";
        case SPRITE_FRUIT_MANGO:  return "Mango";
        
        // Effects
        case SPRITE_SPLASH:     return "Splash";
        case SPRITE_POINTS_200: return "200pts";
        case SPRITE_POINTS_400: return "400pts";
        case SPRITE_POINTS_800: return "800pts";
        
        // UI
        case SPRITE_LIFE: return "Life";
        case SPRITE_LOGO: return "Logo";

        // Catch-all
        case SPRITE_COUNT:
        default:
            return "Unknown";
    }
}